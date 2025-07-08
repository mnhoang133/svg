#ifndef SVGParser_h
#define SVGParser_h

#include <vector>
#include <string>
#include "SVGElement.h"

class SVGParser {
public:
    static std::vector<SVGElement*> parseFile(const std::string& filename);
};


#endif // !SVGParser_H
