//todo: cleanup the code
//todo: visibility test is still buggy??
//todo: add fuzzy search
//todo: handle document memory leak (because documents are not deleted since adding state history)
//todo: tests!
//todo: handle right to left documents
//todo: clean up parsing code
//todo: autocomplete in command window
//todo: add djvu, and other formats
//todo: simplify word selection logic (also avoid inefficient extra insertions followed by clears in selected_characters)
//todo: make it so that find_closest_bookmark and find_closest_link return the index instead of pointer
// we are only using the index anyway and returning a pointer has potential for misuse.
//todo: back after clicking on the first link in a document is not working properly
//todo: make it so that all commands that change document state (for example goto_offset_withing_page, goto_link, etc.) do not change the document
// state, instead they return a DocumentViewState object that is then applied using push_state and chnage_state functions
// (chnage state should be a function that just applies the state without pushing it to history)
//todo: add "repeat last command" command
//todo: make it so that when you middle click on the name of a paper, we search it in google scholar.
//todo: add support for smart screen fit
//todo: get_page_width_smart is still not perfect

#include <iostream>
#include <vector>
#include <string>
#include <regex>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <optional>
#include <utility>
#include <memory>

#include <qapplication.h>
#include <qpushbutton.h>
#include <qopenglwidget.h>
#include <qopenglextrafunctions.h>
#include <qopenglfunctions.h>
#include <qopengl.h>
#include <qwindow.h>
#include <qkeyevent.h>
#include <qlineedit.h>
#include <qtreeview.h>
#include <qsortfilterproxymodel.h>
#include <qabstractitemmodel.h>
#include <qopenglshaderprogram.h>
#include <qtimer.h>
#include <qdatetime.h>
#include <qstackedwidget.h>
#include <qboxlayout.h>
#include <qlistview.h>
#include <qstringlistmodel.h>
#include <qlabel.h>
#include <qtextedit.h>
#include <qfilesystemwatcher.h>
#include <qdesktopwidget.h>
#include <qfontdatabase.h>
#include <qstandarditemmodel.h>
#include <qscrollarea.h>
#include <qdesktopservices.h>
#include <qprocess.h>

//#include <Windows.h>
#include <mupdf/fitz.h>
#include "sqlite3.h"
#include <filesystem>


#include "input.h"
#include "database.h"
#include "book.h"
#include "utils.h"
#include "ui.h"
#include "pdf_renderer.h"
#include "document.h"
#include "document_view.h"
#include "pdf_view_opengl_widget.h"
#include "config.h"
#include "utf8.h"
#include "main_widget.h"

#define FTS_FUZZY_MATCH_IMPLEMENTATION
#include "fts_fuzzy_match.h"
#undef FTS_FUZZY_MATCH_IMPLEMENTATION

#ifndef MAX_PATH
#define MAX_PATH PATH_MAX
#endif


extern float background_color[3] = { 1.0f, 1.0f, 1.0f };
extern float ZOOM_INC_FACTOR = 1.2f;
extern float vertical_move_amount = 1.0f;
extern float horizontal_move_amount = 1.0f;
extern float vertical_line_width = 0.1f;
extern float vertical_line_freq = 0.001f;
extern float move_screen_percentage = 0.8f;
extern const unsigned int cache_invalid_milies = 1000;
extern const int persist_milies = 1000 * 60;
extern const int page_paddings = 0;
extern const int max_pending_requests = 31;
extern bool launched_from_file_icon = false;
extern bool flat_table_of_contents = false;
extern bool should_use_multiple_monitors = false;
extern std::wstring libgen_address = L"";
extern std::wstring google_scholar_address = L"";

extern std::filesystem::path last_path_file_absolute_location = "";
extern std::filesystem::path parent_path = "";


std::mutex mupdf_mutexes[FZ_LOCK_MAX];

void lock_mutex(void* user, int lock) {
	std::mutex* mut = (std::mutex*)user;
	(mut + lock)->lock();
}

void unlock_mutex(void* user, int lock) {
	std::mutex* mut = (std::mutex*)user;
	(mut + lock)->unlock();
}


int main(int argc, char* args[]) {

	QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts, true);
	QApplication app(argc, args);

	parent_path = QCoreApplication::applicationDirPath().toStdWString();
	std::string exe_path = utf8_encode(QCoreApplication::applicationFilePath().toStdWString());

#ifdef NDEBUG
	install_app(exe_path.c_str());
#else
	std::filesystem::path source_file_path = __FILE__;
	parent_path = source_file_path.parent_path();
#endif

	last_path_file_absolute_location = (parent_path / "last_document_path.txt").wstring();

	std::filesystem::path config_path = parent_path / "prefs.config";
	ConfigManager config_manager(config_path);

	sqlite3* db;
	char* error_message = nullptr;
	int rc;

	rc = sqlite3_open((parent_path / "test.db").string().c_str(), &db);
	if (rc) {
		std::cerr << "could not open database" << sqlite3_errmsg(db) << std::endl;
	}

	create_tables(db);

	fz_locks_context locks;
	locks.user = mupdf_mutexes;
	locks.lock = lock_mutex;
	locks.unlock = unlock_mutex;

	fz_context* mupdf_context = fz_new_context(nullptr, &locks, FZ_STORE_UNLIMITED);

	if (!mupdf_context) {
		std::cerr << "could not create mupdf context" << std::endl;
		return -1;
	}
	bool fail = false;
	fz_try(mupdf_context) {
		fz_register_document_handlers(mupdf_context);
	}
	fz_catch(mupdf_context) {
		std::cerr << "could not register document handlers" << std::endl;
		fail = true;
	}

	if (fail) {
		return -1;
	}

	bool quit = false;

	std::filesystem::path keymap_path = parent_path / "keys.config";
	InputHandler input_handler(keymap_path);

	//char file_path[MAX_PATH] = { 0 };
	std::wstring file_path;
	std::string file_path_;
	std::ifstream last_state_file(last_path_file_absolute_location);
	std::getline(last_state_file, file_path_);
	file_path = utf8_decode(file_path_);
	last_state_file.close();

	launched_from_file_icon = false;
	if (argc > 1) {
		file_path = app.arguments().at(1).toStdWString();
		launched_from_file_icon = true;
	}

	DocumentManager document_manager(mupdf_context, db);

	QFileSystemWatcher pref_file_watcher;
	pref_file_watcher.addPath(QString::fromStdWString(config_path.wstring()));


	QFileSystemWatcher key_file_watcher;
	key_file_watcher.addPath(QString::fromStdWString(keymap_path.wstring()));


	QString font_path = QString::fromStdWString((parent_path / "fonts" / "monaco.ttf").wstring());
	if (QFontDatabase::addApplicationFont(font_path) < 0) {
		std::wcout << "could not add font!" << endl;
	}

	QIcon icon(QString::fromStdWString((parent_path / "icon2.ico").wstring()));
	app.setWindowIcon(icon);

	MainWidget main_widget(mupdf_context, db, &document_manager, &config_manager, &input_handler, &quit);
	main_widget.open_document(file_path);
	main_widget.resize(500, 500);
	main_widget.showMaximized();

	// live reload the config file
	QObject::connect(&pref_file_watcher, &QFileSystemWatcher::fileChanged, [&]() {
		std::wifstream config_file(config_path);
		config_manager.deserialize(config_file);
		config_file.close();
		ConfigFileChangeListener::notify_config_file_changed(&config_manager);
		main_widget.validate_render();
		});

	QObject::connect(&key_file_watcher, &QFileSystemWatcher::fileChanged, [&]() {
		input_handler.reload_config_file(keymap_path);
		});

	app.exec();

	quit = true;

	return 0;
}
