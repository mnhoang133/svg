#include "stdafx.h"           // luon de dau tien neu xai precompiled header

#include "SVGGroup.h"
#include "SVGElement.h"       // backup neu .cpp can dung kieu SVGElement*
#include <gdiplus.h>          // can de dung Gdiplus::Graphics

using namespace Gdiplus;

// Ham khoi tao SVGGroup tu danh sach cac element con
SVGGroup::SVGGroup(const std::vector<SVGElement*>& elements)
    : children(elements) {
}

// Ham them mot element moi vao nhom
void SVGGroup::addElement(SVGElement* element) {
    children.push_back(element);
}

// Ham ve cac element trong group
void SVGGroup::render(Graphics* graphics) {
    for (SVGElement* element : children) {
        if (element) {
            // Goi ham render cua tung element
            element->render(graphics);
        }
    }
}

// Ham huy doi tuong -> giai phong bo nho cac element con
SVGGroup::~SVGGroup() {
    for (SVGElement* element : children) {
        delete element;
    }
    children.clear(); // Xoa danh sach sau khi giai phong
}

int SVGGroup::childCount()
{
    return children.size();
}