#ifndef SVGGROUP_H
#define SVGGROUP_H

#include "SVGElement.h"
#include <vector>

//  Forward declare Graphics vì chỉ dùng pointer
namespace Gdiplus {
    class Graphics;
}

class SVGGroup : public SVGElement {
private:
    std::vector<SVGElement*> children;

public:
    // Constructor nhận vector SVGElement* đã tạo
    SVGGroup(const std::vector<SVGElement*>& elements);

    // Thêm phần tử mới vào nhóm
    void addElement(SVGElement* element);

    // Gọi render cho toàn bộ phần tử con
    void render(Gdiplus::Graphics* graphics) override;

    int childCount();

    // Hủy các phần tử nếu cần
    ~SVGGroup();
};

#endif 
