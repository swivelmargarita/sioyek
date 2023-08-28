#pragma once

#include <qpoint.h>


class Document;
class DocumentView;

struct AbsoluteDocumentPos;
struct NormalizedWindowPos;
struct WindowPos;

struct DocumentPos {
    int page;
    float x;
    float y;

    AbsoluteDocumentPos to_absolute(Document* doc);
    NormalizedWindowPos to_window_normalized(DocumentView* document_view);
    WindowPos to_window(DocumentView* document_view);
};

struct AbsoluteDocumentPos {
    float x;
    // this is the concatenated y-coordinate of the current page (sum of all page heights up to current location)
    float y;

    DocumentPos to_document(Document* doc);
    NormalizedWindowPos to_window_normalized(DocumentView* document_view);
    WindowPos to_window(DocumentView* document_view);
};

// normalized window coordinates. x and y are in the range [-1, 1]
struct NormalizedWindowPos {
    float x;
    float y;

    DocumentPos to_document(DocumentView* document_view);
    AbsoluteDocumentPos to_absolute(DocumentView* document_view);
    WindowPos to_window(DocumentView* document_view);
};

// window coordinate in pixels
struct WindowPos {
    int x;
    int y;

    WindowPos(float x_, float y_);
    WindowPos(int x_, int y_);
    WindowPos();

    DocumentPos to_document(DocumentView* document_view);
    AbsoluteDocumentPos to_absolute(DocumentView* document_view);
    NormalizedWindowPos to_window_normalized(DocumentView* document_view);

};


template<typename T, int dim>
struct Vec {
    T values[dim];

    Vec(const QPoint& p) {
        values[0] = p.x();
        values[1] = p.y();
    }

    Vec(const NormalizedWindowPos& p) {
        values[0] = p.x;
        values[1] = p.y;
    }

    Vec(const WindowPos& p) {
        values[0] = p.x;
        values[1] = p.y;
    }

    Vec(const AbsoluteDocumentPos& p) {
        values[0] = p.x;
        values[1] = p.y;
    }

    Vec(T v1, T v2) {
        values[0] = v1;
        values[1] = v2;
    }

    Vec(T values_[]) : values(values_) {

    }

    Vec() {
        for (int i = 0; i < dim; i++) {
            values[i] = 0;
        }
    }

    T& operator[](int i) {
        return values[i];
    }

    const T& operator[](int i) const {
        return values[i];
    }

    T x() {
        return values[0];
    }

    T y() {
        return values[1];
    }

    T width() {
        return values[0];
    }

    T height() {
        return values[1];
    }

    NormalizedWindowPos to_normalized_window_pos() {
        return NormalizedWindowPos{ values[0], values[1] };
    }
};

using ivec2 = Vec<int, 2>;
using fvec2 = Vec<float, 2>;

template<typename T, int dim>
Vec<float, dim> operator/(const Vec<T, dim>& lhs, float rhs) {
    Vec<float, dim> res;
    for (int i = 0; i < dim; i++) {
        res[i] = lhs[i] / rhs;
    }
    return res;
}

template<typename T, int dim>
Vec<T, dim> operator+(const Vec<T, dim>& lhs, const Vec<T, dim>& rhs) {
    Vec<T, dim> res;
    for (int i = 0; i < dim; i++) {
        res[i] = lhs[i] + rhs[i];
    }
    return res;
}

template<typename T, int dim>
Vec<T, dim> operator-(const Vec<T, dim>& lhs, const Vec<T, dim>& rhs) {
    Vec<T, dim> res;
    for (int i = 0; i < dim; i++) {
        res[i] = lhs[i] - rhs[i];
    }
    return res;
}

//template<typename T, int dim>
//operator+ (Vec<T, dim> a, Vec<T, dim> b) {
//	Vec<T, dim> c;
//	for (int i = 0; i < dim; i++) {
//		c.values[i] = a.values[i] + b.values[i];
//	}
//	return c;
//}
bool are_same(const AbsoluteDocumentPos& lhs, const AbsoluteDocumentPos& rhs);
