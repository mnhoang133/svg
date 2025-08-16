#ifndef MANAGERGRADIENT_H
#define MANAGERGRADIENT_H

#include "SVGGradient.h"
#include <unordered_set>

// Gradient manager để lưu và quản lý các gradient
class GradientManager {
private:
    static std::map<std::string, SVGGradient*> gradients;

public:
    static void addGradient(const std::string& id, SVGGradient* gradient);
    static SVGGradient* getGradient(const std::string& id);
    static void clear();
    static Gdiplus::Brush* createBrushFromUrl(const std::string& url, const Gdiplus::RectF& bounds);
    static void resolveAllGradients();
    static void resolveGradient(const std::string& id, std::unordered_set<std::string>& visited);

};

#endif