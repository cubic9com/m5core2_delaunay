#include "DelaunayTriangulation.h"

// Constructor
DelaunayTriangulation::DelaunayTriangulation(std::vector<Point>& points, M5Canvas& canvas)
    : points(points), canvas(canvas) {
    // Pre-allocate triangle cache to avoid frequent reallocations
    triangleCache.reserve(60);  // Estimate: ~2x MAX_POINTS
}

// Find all Delaunay triangles from the current set of points
void DelaunayTriangulation::findDelaunayTriangles() {
    // Clear previous triangles but keep capacity
    triangleCache.clear();
    
    // Need at least 3 points to form a triangle
    if (points.size() < 3) {
        return;
    }
    
    size_t n = points.size();
    
    // Check all possible triangles
    for (size_t i = 0; i < n - 2; i++) {
        for (size_t j = i + 1; j < n - 1; j++) {
            for (size_t k = j + 1; k < n; k++) {
                Triangle tri(&points[i], &points[j], &points[k]);
                
                // Check if no other points are inside this triangle's circumcircle
                bool isDelaunay = true;
                for (size_t l = 0; l < n; l++) {
                    if (l != i && l != j && l != k) {
                        if (tri.isPointInCircumcircle(points[l])) {
                            isDelaunay = false;
                            break;
                        }
                    }
                }
                
                // Add to cache if it satisfies Delaunay condition
                if (isDelaunay) {
                    triangleCache.push_back(tri);
                }
            }
        }
    }
}

// Calculate and draw the Delaunay triangulation
void DelaunayTriangulation::calculateAndDraw() {
    // Clear canvas
    canvas.fillScreen(BLACK);
    
    // If we have enough points, find and draw triangles
    if (points.size() >= 3) {
        // Find triangles (fills triangleCache)
        findDelaunayTriangles();
        
        // Draw each triangle from cache
        for (const auto& triangle : triangleCache) {
            uint16_t color = ColorUtils::generatePastelColor(triangle.getColorSeed());
            drawTriangle(triangle, color);
        }
    }
    
    // Draw points on top of triangles
    drawPoints();
    
    // Transfer canvas content to display
    canvas.pushSprite(0, 0);
}

// Draw a single triangle with the specified color
void DelaunayTriangulation::drawTriangle(const Triangle& triangle, uint16_t color) {
    // Draw each line with the specified thickness
    // Draw offset in both x and y directions for more uniform thickness
    int thickness = DisplayConstants::LINE_THICKNESS;
    for (int dx = 0; dx < thickness; dx++) {
        for (int dy = 0; dy < thickness; dy++) {
            canvas.drawLine(triangle.p1->x + dx, triangle.p1->y + dy, 
                            triangle.p2->x + dx, triangle.p2->y + dy, color);
            
            canvas.drawLine(triangle.p2->x + dx, triangle.p2->y + dy, 
                            triangle.p3->x + dx, triangle.p3->y + dy, color);
            
            canvas.drawLine(triangle.p3->x + dx, triangle.p3->y + dy, 
                            triangle.p1->x + dx, triangle.p1->y + dy, color);
        }
    }
}

// Draw all points
void DelaunayTriangulation::drawPoints() {
    for (const auto& point : points) {
        canvas.fillCircle(point.x, point.y, DisplayConstants::POINT_RADIUS, WHITE);
    }
}
