#include "DelaunayTriangulation.h"

// Constructor
DelaunayTriangulation::DelaunayTriangulation(std::vector<Point>& points, M5Canvas& canvas)
    : points(points), canvas(canvas) {
}

// Find all Delaunay triangles from the current set of points
std::vector<Triangle> DelaunayTriangulation::findDelaunayTriangles() {
    std::vector<Triangle> delaunayTriangles;
    
    // Need at least 3 points to form a triangle
    if (points.size() < 3) {
        return delaunayTriangles;
    }
    
    // Check all possible triangles
    for (size_t i = 0; i < points.size() - 2; i++) {
        for (size_t j = i + 1; j < points.size() - 1; j++) {
            for (size_t k = j + 1; k < points.size(); k++) {
                Triangle tri(&points[i], &points[j], &points[k]);
                
                // Check if no other points are inside this triangle's circumcircle
                bool isDelaunay = true;
                for (size_t l = 0; l < points.size(); l++) {
                    if (l != i && l != j && l != k) {
                        if (tri.isPointInCircumcircle(points[l])) {
                            isDelaunay = false;
                            break;
                        }
                    }
                }
                
                // Add to list if it satisfies Delaunay condition
                if (isDelaunay) {
                    delaunayTriangles.push_back(tri);
                }
            }
        }
    }
    
    return delaunayTriangles;
}

// Calculate and draw the Delaunay triangulation
void DelaunayTriangulation::calculateAndDraw() {
    // Clear canvas
    canvas.fillScreen(BLACK);
    
    // If we have enough points, find and draw triangles
    if (points.size() >= 3) {
        std::vector<Triangle> triangles = findDelaunayTriangles();
        
        // Draw each triangle
        for (const auto& triangle : triangles) {
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
    for (int i = 0; i < DisplayConstants::LINE_THICKNESS; i++) {
        canvas.drawLine(triangle.p1->x + i, triangle.p1->y, 
                        triangle.p2->x + i, triangle.p2->y, color);
        
        canvas.drawLine(triangle.p2->x + i, triangle.p2->y, 
                        triangle.p3->x + i, triangle.p3->y, color);
        
        canvas.drawLine(triangle.p3->x + i, triangle.p3->y, 
                        triangle.p1->x + i, triangle.p1->y, color);
    }
}

// Draw all points
void DelaunayTriangulation::drawPoints() {
    for (const auto& point : points) {
        canvas.fillCircle(point.x, point.y, DisplayConstants::POINT_RADIUS, WHITE);
    }
}
