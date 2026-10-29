#pragma once

#include <cstdint>
#include <utility>
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
    const uint32_t& pointsVersion;  // Point set version, owned by PointManager
    
    // Reusable buffer for triangles to avoid reallocations
    std::vector<Triangle> triangleCache;

    // Scratch buffers for the Bowyer-Watson algorithm, reused between
    // recomputations to avoid repeated allocations
    std::vector<Triangle> workTriangles;
    std::vector<size_t> badTriangleIndices;
    std::vector<char> badTriangleFlags;
    std::vector<std::pair<Point*, Point*>> boundaryEdges;

    // A unique edge of the triangulation, with the color of the first
    // triangle that used it
    struct EdgeInfo {
        Point* a;
        Point* b;
        uint16_t color;
    };
    std::vector<EdgeInfo> edgeCache;

    // Vertices of the super-triangle used by the Bowyer-Watson algorithm
    Point superPoint1;
    Point superPoint2;
    Point superPoint3;

    // Topology cache state: the points move only slightly between frames,
    // so the triangulation is recomputed only every
    // TriangulationConstants::RECOMPUTE_INTERVAL_FRAMES frames, or
    // immediately when the point set changes
    int framesSinceTriangulation;
    uint32_t lastPointsVersion;
    size_t lastPointCount;
    bool hasCachedTopology;

    // Find all Delaunay triangles from the current set of points
    // (Bowyer-Watson incremental algorithm)
    void findDelaunayTriangles();

    // Draw each unique edge of the cached triangles once
    void drawUniqueEdges();

public:
    // Constructor
    DelaunayTriangulation(std::vector<Point>& points, M5Canvas& canvas, const uint32_t& pointsVersion);

    // Calculate and draw the Delaunay triangulation
    void calculateAndDraw();

    // Draw a single triangle with the specified color
    void drawTriangle(const Triangle& triangle, uint16_t color);

    // Draw all points
    void drawPoints();
};
