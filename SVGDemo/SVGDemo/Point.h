
#ifndef Point_h
#define Point_h

namespace svg
{
    struct Point {
        float x = 0;
        float y = 0;

        Point() = default;
        Point(float xVal, float yVal) : x(xVal), y(yVal) {}

        void move(float dx, float dy)
        {
            x += dx;
            y += dy;
        }
    };
}

#endif
