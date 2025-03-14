#pragma once

#include <vector>
#include <M5Unified.h>
#include "Point.h"
#include "Constants.h"

// Class for managing points and their behavior
class PointManager {
private:
    std::vector<Point>& points;
    Point* lastAddedPoint;
    
    // Apply Brownian motion to all points
    void applyBrownianMotion();
    
    // Apply constraints to keep points near their original positions
    void applyPositionConstraints();

public:
    // Constructor
    PointManager(std::vector<Point>& points);
    
    // Add a new point at the specified coordinates
    Point* addPoint(float x, float y);
    
    // Apply repulsion from a newly added point to all other points
    void applyRepulsion(Point* newPoint);
    
    // Update all points' positions based on their velocities
    void updatePoints(int screenWidth, int screenHeight);
    
    // Get the vector of points
    std::vector<Point>& getPoints() { return points; }
    
    // Get the last added point
    Point* getLastAddedPoint() { return lastAddedPoint; }
};
