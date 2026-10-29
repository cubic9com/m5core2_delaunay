#include "DelaunayTriangulation.h"
#include <algorithm>

// Constructor
DelaunayTriangulation::DelaunayTriangulation(std::vector<Point>& points, M5Canvas& canvas, const uint32_t& pointsVersion)
    : points(points), canvas(canvas), pointsVersion(pointsVersion),
      superPoint1(0, 0), superPoint2(0, 0), superPoint3(0, 0),
      framesSinceTriangulation(0), lastPointsVersion(0), lastPointCount(0),
      hasCachedTopology(false) {
    // Pre-allocate caches to avoid frequent reallocations
    triangleCache.reserve(60);  // Estimate: ~2x MAX_POINTS
    workTriangles.reserve(70);
    edgeCache.reserve(90);
}

// Find all Delaunay triangles from the current set of points
// (Bowyer-Watson incremental algorithm: insert the points one by one,
// removing the triangles whose circumcircle contains the new point and
// retriangulating the hole that remains)
void DelaunayTriangulation::findDelaunayTriangles() {
    // Clear previous triangles but keep capacity
    triangleCache.clear();
    
    // Need at least 3 points to form a triangle
    if (points.size() < 3) {
        return;
    }
    
    // Bounding box of the point set
    float minX = points[0].x, maxX = points[0].x;
    float minY = points[0].y, maxY = points[0].y;
    for (const auto& p : points) {
        minX = std::min(minX, p.x);
        maxX = std::max(maxX, p.x);
        minY = std::min(minY, p.y);
        maxY = std::max(maxY, p.y);
    }
    
    // Super-triangle that surely contains all points (its vertices are
    // removed again at the end). The margin has to be very large: the
    // circumcircle of a flat triangle near the convex hull can extend
    // far beyond the point set, and a super-triangle vertex inside such
    // a circumcircle would suppress the real hull triangle.
    float dmax = std::max(maxX - minX, maxY - minY);
    if (dmax < 1.0f) {
        dmax = 1.0f;
    }
    const float midX = (minX + maxX) * 0.5f;
    const float midY = (minY + maxY) * 0.5f;
    superPoint1.x = midX - 2000.0f * dmax;
    superPoint1.y = midY - dmax;
    superPoint2.x = midX;
    superPoint2.y = midY + 2000.0f * dmax;
    superPoint3.x = midX + 2000.0f * dmax;
    superPoint3.y = midY - dmax;
    
    // Start with the super-triangle
    workTriangles.clear();
    workTriangles.emplace_back(&superPoint1, &superPoint2, &superPoint3);
    
    // Insert the points one by one
    for (auto& point : points) {
        // Find the triangles whose circumcircle contains the point
        badTriangleIndices.clear();
        for (size_t t = 0; t < workTriangles.size(); t++) {
            if (workTriangles[t].isPointInCircumcircle(point)) {
                badTriangleIndices.push_back(t);
            }
        }
        if (badTriangleIndices.empty()) {
            continue;
        }
        
        badTriangleFlags.assign(workTriangles.size(), 0);
        for (size_t index : badTriangleIndices) {
            badTriangleFlags[index] = 1;
        }
        
        // The boundary of the polygonal hole is the set of edges that
        // belong to exactly one bad triangle
        boundaryEdges.clear();
        for (size_t index : badTriangleIndices) {
            const Triangle& tri = workTriangles[index];
            Point* vertices[3] = {tri.p1, tri.p2, tri.p3};
            for (int e = 0; e < 3; e++) {
                Point* a = vertices[e];
                Point* b = vertices[(e + 1) % 3];
                // Canonical direction, so the same edge always matches
                if (b < a) {
                    std::swap(a, b);
                }
                bool duplicate = false;
                for (size_t k = 0; k < boundaryEdges.size(); k++) {
                    if (boundaryEdges[k].first == a && boundaryEdges[k].second == b) {
                        boundaryEdges.erase(boundaryEdges.begin() + k);
                        duplicate = true;
                        break;
                    }
                }
                if (!duplicate) {
                    boundaryEdges.emplace_back(a, b);
                }
            }
        }
        
        // Remove the bad triangles
        size_t kept = 0;
        for (size_t t = 0; t < workTriangles.size(); t++) {
            if (!badTriangleFlags[t]) {
                workTriangles[kept] = workTriangles[t];
                kept++;
            }
        }
        workTriangles.erase(workTriangles.begin() + kept, workTriangles.end());
        
        // Retriangulate the hole with the inserted point
        for (auto& edge : boundaryEdges) {
            workTriangles.emplace_back(edge.first, edge.second, &point);
        }
    }
    
    // Keep only the triangles that do not use a super-triangle vertex
    for (auto& tri : workTriangles) {
        if (tri.p1 == &superPoint1 || tri.p1 == &superPoint2 || tri.p1 == &superPoint3 ||
            tri.p2 == &superPoint1 || tri.p2 == &superPoint2 || tri.p2 == &superPoint3 ||
            tri.p3 == &superPoint1 || tri.p3 == &superPoint2 || tri.p3 == &superPoint3) {
            continue;
        }
        triangleCache.push_back(tri);
    }
}

// Calculate and draw the Delaunay triangulation
void DelaunayTriangulation::calculateAndDraw() {
    // Clear canvas
    canvas.fillScreen(BLACK);
    
    // If we have enough points, find and draw triangles
    if (points.size() >= 3) {
        // Recompute the topology only when the point set changed or the
        // recompute interval has elapsed; the drawing below always uses
        // the live point positions, so the lines keep following the points
        framesSinceTriangulation++;
        const bool pointSetChanged = !hasCachedTopology ||
                                     pointsVersion != lastPointsVersion ||
                                     points.size() != lastPointCount;
        if (pointSetChanged ||
            framesSinceTriangulation >= TriangulationConstants::RECOMPUTE_INTERVAL_FRAMES) {
            findDelaunayTriangles();
            hasCachedTopology = true;
            lastPointsVersion = pointsVersion;
            lastPointCount = points.size();
            framesSinceTriangulation = 0;
        }
        
        // Draw each unique edge once
        drawUniqueEdges();
    }
    
    // Draw points on top of triangles
    drawPoints();
    
    // Transfer canvas content to display
    canvas.pushSprite(0, 0);
}

// Draw each unique edge of the cached triangles once (adjacent triangles
// share edges, so drawing triangle by triangle would draw every inner
// edge twice)
void DelaunayTriangulation::drawUniqueEdges() {
    edgeCache.clear();
    
    // Collect the unique edges, keeping the color of the first triangle
    // that uses each edge
    for (const auto& triangle : triangleCache) {
        const uint16_t color = ColorUtils::generatePastelColor(triangle.getColorSeed());
        Point* vertices[3] = {triangle.p1, triangle.p2, triangle.p3};
        for (int e = 0; e < 3; e++) {
            Point* a = vertices[e];
            Point* b = vertices[(e + 1) % 3];
            // Canonical direction, so the same edge always matches
            if (b < a) {
                std::swap(a, b);
            }
            bool exists = false;
            for (const auto& edge : edgeCache) {
                if (edge.a == a && edge.b == b) {
                    exists = true;
                    break;
                }
            }
            if (!exists) {
                edgeCache.push_back({a, b, color});
            }
        }
    }
    
    // Draw each edge with the specified thickness
    // Draw offset in both x and y directions for more uniform thickness
    const int thickness = DisplayConstants::LINE_THICKNESS;
    for (const auto& edge : edgeCache) {
        for (int dx = 0; dx < thickness; dx++) {
            for (int dy = 0; dy < thickness; dy++) {
                canvas.drawLine(edge.a->x + dx, edge.a->y + dy,
                                edge.b->x + dx, edge.b->y + dy, edge.color);
            }
        }
    }
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
