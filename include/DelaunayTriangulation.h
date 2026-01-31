#pragma once

#include <vector>
#include <M5Unified.h>
#include <M5GFX.h>
#include "Point.h"
#include "Triangle.h"
#include "ColorUtils.h"
#include "Constants.h"

// Class for managing Delaunay triangulation
class DelaunayTriangulation {
private:
    std::vector<Point>& points;
    M5Canvas& canvas;
    
    // Reusable buffer for triangles to avoid reallocations
    std::vector<Triangle> triangleCache;

    // Find all Delaunay triangles from the current set of points
    void findDelaunayTriangles();

public:
    // Constructor
    DelaunayTriangulation(std::vector<Point>& points, M5Canvas& canvas);

    // Calculate and draw the Delaunay triangulation
    void calculateAndDraw();

    // Draw a single triangle with the specified color
    void drawTriangle(const Triangle& triangle, uint16_t color);

    // Draw all points
    void drawPoints();
};
