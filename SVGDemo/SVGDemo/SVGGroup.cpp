#include "SVGGroup.h"

SVGGroup::SVGGroup(const std::vector<SVGElement*>& elements)
    : children(elements) {}

void SVGGroup::addElement(SVGElement* element) {
    children.push_back(element);
}

void SVGGroup::render(Gdiplus::Graphics* graphics) {
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
