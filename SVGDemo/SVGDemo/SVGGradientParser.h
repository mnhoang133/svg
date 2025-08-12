#ifndef SVGGRADIENTPARSER_H
#define SVGGRADIENTPARSER_H

#include <string>
#include "SVGGradient.h"
#include "SVGLinearGradient.h"
#include "SVGRadialGradient.h"

class SVGGradientParser {
public:
    // Parse defs section và extract tất cả gradients
    static void parseDefs(const std::string& defsContent);

    // Parse một linear gradient element
    static SVGLinearGradient* parseLinearGradient(const std::string& gradientTag);

    // Parse một radial gradient element
    static SVGRadialGradient* parseRadialGradient(const std::string& gradientTag);

    // Parse gradient stops từ nội dung bên trong gradient tag
    static void parseGradientStops(const std::string& gradientContent, SVGGradient* gradient);

    // Parse một stop element
    static GradientStop parseStop(const std::string& stopTag);

    // Helper: kiểm tra xem fill attribute có phải là gradient URL không
    static bool isFillGradientUrl(const std::string& fillValue);

    // Helper: extract gradient ID từ URL
    static std::string extractGradientId(const std::string& url);

private:
    // Parse coordinate value (có thể là % hoặc absolute)
    static float parseCoordinate(const std::string& value, float defaultVal = 0.0f);

    // Parse stop-opacity từ style hoặc attribute
    static float parseStopOpacity(const std::string& stopTag);
};

#endif
