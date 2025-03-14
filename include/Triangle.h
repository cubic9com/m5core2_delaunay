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
        // Three points of the triangle
        float x1 = p1->x;
        float y1 = p1->y;
        float x2 = p2->x;
        float y2 = p2->y;
        float x3 = p3->x;
        float y3 = p3->y;
        
        // Calculate determinant
        float a = x1 * (y2 - y3) - y1 * (x2 - x3) + x2 * y3 - x3 * y2;
        
        // Handle degenerate case (colinear points)
        if (fabs(a) < 1e-6) return false;
        
        float b = (x1 * x1 + y1 * y1) * (y3 - y2) + 
                  (x2 * x2 + y2 * y2) * (y1 - y3) + 
                  (x3 * x3 + y3 * y3) * (y2 - y1);
        
        float c = (x1 * x1 + y1 * y1) * (x2 - x3) + 
                  (x2 * x2 + y2 * y2) * (x3 - x1) + 
                  (x3 * x3 + y3 * y3) * (x1 - x2);
        
        float d = (x1 * x1 + y1 * y1) * (x3 * y2 - x2 * y3) + 
                  (x2 * x2 + y2 * y2) * (x1 * y3 - x3 * y1) + 
                  (x3 * x3 + y3 * y3) * (x2 * y1 - x1 * y2);
        
        // Center and squared radius of circumcircle
        float centerX = -b / (2 * a);
        float centerY = -c / (2 * a);
        float radiusSquared = (b * b + c * c - 4 * a * d) / (4 * a * a);
        
        // Squared distance between point and center
        float distSq = (p.x - centerX) * (p.x - centerX) + 
                       (p.y - centerY) * (p.y - centerY);
        
        // Inside circle if distance is less than radius
        return distSq < radiusSquared;
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
