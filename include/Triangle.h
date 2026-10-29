#pragma once

#include "Point.h"

// Triangle class representing a triangle formed by three points
class Triangle {
public:
    Point* p1;
    Point* p2;
    Point* p3;

    // Constructor
    Triangle(Point* _p1, Point* _p2, Point* _p3) 
        : p1(_p1), p2(_p2), p3(_p3) {}

    // Check if a point is inside the circumcircle of this triangle
    bool isPointInCircumcircle(const Point& p) const {
        // Incircle test in determinant form, using coordinates relative
        // to the test point. Unlike the circumcenter/circumradius form,
        // this needs no division and stays numerically accurate for
        // points far from the origin, because all values are small
        // differences of coordinates.
        const float ax = p1->x - p.x;
        const float ay = p1->y - p.y;
        const float bx = p2->x - p.x;
        const float by = p2->y - p.y;
        const float cx = p3->x - p.x;
        const float cy = p3->y - p.y;
        
        const float det = (ax * ax + ay * ay) * (bx * cy - cx * by)
                        - (bx * bx + by * by) * (ax * cy - cx * ay)
                        + (cx * cx + cy * cy) * (ax * by - bx * ay);
        
        // Signed area of the triangle (zero for collinear points, which
        // have no circumcircle)
        const float orient = (p2->x - p1->x) * (p3->y - p1->y)
                           - (p2->y - p1->y) * (p3->x - p1->x);
        if (orient == 0.0f) {
            return false;
        }
        
        return orient > 0.0f ? det > 0.0f : det < 0.0f;
    }

    // Generate a unique seed for this triangle (for consistent coloring)
    uint32_t getColorSeed() const {
        // Use memory addresses of points to create a unique seed
        uintptr_t addr1 = reinterpret_cast<uintptr_t>(p1);
        uintptr_t addr2 = reinterpret_cast<uintptr_t>(p2);
        uintptr_t addr3 = reinterpret_cast<uintptr_t>(p3);
        
        // Combine addresses to create a seed
        return static_cast<uint32_t>((addr1 * 73856093) ^ 
                                    (addr2 * 19349663) ^ 
                                    (addr3 * 83492791));
    }
};
