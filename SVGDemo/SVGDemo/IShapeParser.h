#ifndef IShapeParser_h
#define IShapeParser_h

#include <string>
#include "SVGElement.h"

//Interface để parsing các shape như circle, rect, line, path, ...
class IShapeParser
{
public:
	IShapeParser() = default;
	virtual SVGElement* parse(const std::string& tag) const = 0; // parse kiểu tag, ví dụ tag truyền vào là circle thì sẽ dành cho circle
	virtual ~IShapeParser() = default;
};

#endif // !IShapeParser_h
