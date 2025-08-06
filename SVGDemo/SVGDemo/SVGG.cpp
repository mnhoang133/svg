#include "stdafx.h"
#include "SVGG.h"
#include "gdiplus.h"

// Constructor mặc định
SVGG::SVGG() : strokeWidth(1.0f), strokeOpacity(1.0f), fillOpacity(1.0f) {
    // Khởi tạo màu mặc định: đen viền, trắng nền
    strokeColor = Gdiplus::Color(255, 0, 0, 0);   // opaque black
    fillColor = Gdiplus::Color(255, 255, 255, 255); // opaque white
}

// Đặt style cho nhóm
void SVGG::setStyle(Gdiplus::Color stroke, float strokeW, float strokeOp,
    Gdiplus::Color fill, float fillOp) {
    strokeColor = stroke;
    strokeWidth = strokeW;
    strokeOpacity = strokeOp;
    fillColor = fill;
    fillOpacity = fillOp;
}

// Thêm phần tử con vào nhóm
void SVGG::addChild(SVGElement* shape) {
    if (shape && shape != this) {
        children.push_back(shape);
    }
}

// Hàm render gọi render từng phần tử con, áp dụng transform cha
void SVGG::render(Gdiplus::Graphics* graphics) {


    if (!graphics) return;

    // Lưu trạng thái cũ
    Gdiplus::GraphicsState state = graphics->Save();

    //// Áp dụng transform nếu có
    applyTransform(graphics);

    // Render từng phần tử con
    for (SVGElement* child : children) {
        if (child && child != this) {
            child->transformSetter(this->getTransform());
            child->render(graphics);
        }
    }

    // Khôi phục trạng thái
    graphics->Restore(state);
}

// Destructor: không xóa con ở đây, parser quản lý bộ nhớ
SVGG::~SVGG() {
    // Nếu muốn tự quản lý bộ nhớ:
    // for (SVGElement* child : children) {
    //     delete child;
    // }
    // children.clear();
}
