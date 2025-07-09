#include "stdafx.h"           //  luôn để đầu tiên nếu xài precompiled header

#include "SVGGroup.h"
#include "SVGElement.h"       // backup nếu .cpp cần dùng kiểu SVGElement*
#include <gdiplus.h>          // cần để dùng Gdiplus::Graphics

using namespace Gdiplus;

SVGGroup::SVGGroup(const std::vector<SVGElement*>& elements)
    : children(elements) {}

void SVGGroup::addElement(SVGElement* element) {
    children.push_back(element);
}

void SVGGroup::render(Graphics* graphics) {
    for (SVGElement* element : children) {
        if (element) {
            element->render(graphics);
        }
    }
}

SVGGroup::~SVGGroup() {
    for (SVGElement* element : children) {
        delete element;
    }
    children.clear();
}
