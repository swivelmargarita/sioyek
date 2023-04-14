import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.1
import QtQuick.Controls.Material

import "qrc:/pdf_viewer/touchui"


ColumnLayout{
    spacing: 0

    signal selectTextClicked();
    signal gotoPageClicked();
    signal tocClicked();
    signal searchClicked();
    signal fullscreenClicked();
    signal bookmarksClicked();
    signal highlightsClicked();
    signal rulerModeClicked();
    signal lightColorschemeClicked();
    signal darkColorschemeClicked();
    signal customColorschemeClicked();
    signal openPrevDocClicked();
    signal openNewDocClicked();
    signal commandsClicked();
    signal settingsClicked();

    Rectangle{
        anchors.fill: parent
        z: -1
        color: "#222"
        radius: 10
    }
    Item{

        // color: "yellow"
        // radius: 10
        // height: parent.height / 6
        Layout.preferredHeight: Math.max(parent.height / 6, 100)
        Layout.preferredWidth: parent.width
        // anchors.left: parent.left
        // anchors.right: parent.right
        Column{
            anchors.fill: parent


            Text{
                id: label
                text: "Tools"
                color: "white"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.margins: 10
            }

            TouchButtonGroup{
                buttons: ["qrc:/icons/select_text.svg", "qrc:/icons/page.svg", "📗", "🔎", "↕", "📑", "✍", "📏"]
                tips: ["Select Text", "Goto Page", "Table of Contents", "Search", "Fullscreen", "Bookmarks", "Highlights", "Toggle Ruler Mode"]

                anchors.bottom: parent.bottom
                anchors.top: label.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 10
                onButtonClicked: function (index, name){
                    switch (index){
                        case 0:
                            /* emit */ selectTextClicked(); break;
                        case 1:
                            /* emit */ gotoPageClicked(); break;
                        case 2:
                            /* emit */ tocClicked(); break;
                        case 3:
                            /* emit */ searchClicked(); break;
                        case 4:
                            /* emit */ fullscreenClicked(); break;
                        case 5:
                            /* emit */ bookmarksClicked(); break;
                        case 6:
                            /* emit */ highlightsClicked(); break;
                        case 7:
                            /* emit */ rulerModeClicked(); break;
                        default:
                    }
                }

            }
        }
    }

    Item{

        // color: "yellow"
//        radius: 10
        // Layout.preferredHeight: 5 * parent.height / 18
        Layout.fillHeight: true
        Layout.preferredWidth: parent.width
        // height: 5 * parent.height / 18
        // anchors.left: parent.left
        // anchors.right: parent.right
        Column{
            anchors.fill: parent


            Text{
                id: label0
                text: "Color Scheme"
                color: "white"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.margins: 10
                //            color: "#00B2FF"
            }

            TouchButtonGroup{
                buttons: ["Light", "Dark", "Custom"]
                radio: true
                selectedIndex: _currentColorschemeIndex
                anchors.bottom: parent.bottom
                anchors.top: label0.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 10
                onButtonClicked: function(index, name){
                    if (index == 0){
                        lightColorschemeClicked();
                    }
                    if (index == 1){
                        darkColorschemeClicked();
                    }
                    if (index == 2){
                        customColorschemeClicked();
                    }
                }

            }
        }
    }

    Item{

        // color: "yellow"
//        radius: 10
        // height: 5 * parent.height / 18
        // Layout.preferredHeight: 5 * parent.height / 18
        Layout.fillHeight: true
        Layout.preferredWidth: parent.width
        // anchors.left: parent.left
        // anchors.right: parent.right
        Column{
            anchors.fill: parent


            Text{
                id: label2
                color: "white"
                text: "Open Document"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.margins: 10
                //            color: "#00B2FF"
            }

            TouchButtonGroup{
                buttons: ["Previous", "New"]
                anchors.bottom: parent.bottom
                anchors.top: label2.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 10

                onButtonClicked: function (index, name){
                    if (index == 0){
                        openPrevDocClicked();
                    }
                    if (index == 1){
                        openNewDocClicked();
                    }
                }

            }
        }
    }

    Item{

        // color: "yellow"
//        radius: 10
        // height: 5 * parent.height / 18
        // Layout.preferredHeight: 5 * parent.height / 18
        Layout.fillHeight: true
        Layout.preferredWidth: parent.width
        // anchors.left: parent.left
        // anchors.right: parent.right
        Column{
            anchors.fill: parent


            Text{
                id: label3
                color: "white"
                text: "Advanced"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.margins: 10
                //            color: "#00B2FF"
            }

            TouchButtonGroup{
                buttons: ["Commands", "Settings"]
                anchors.bottom: parent.bottom
                anchors.top: label3.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 10
                onButtonClicked: function(index, name){
                    if (index == 0){
                        commandsClicked();
                    }
                    if (index == 1){
                        settingsClicked();
                    }
                }

            }
        }
    }

}


