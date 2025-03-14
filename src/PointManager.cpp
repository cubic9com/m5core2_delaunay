#include "../include/PointManager.h"

// Constructor
PointManager::PointManager(std::vector<Point>& points)
    : points(points), lastAddedPoint(nullptr) {
}

// Add a new point at the specified coordinates
Point* PointManager::addPoint(float x, float y) {
    // If maximum number of points is reached, remove the oldest point
    if (points.size() >= Config::MAX_POINTS) {
        points.erase(points.begin());  // Remove the oldest point (first in vector)
    }
    
    // Add new point
    points.emplace_back(x, y);
    
    // Store reference to the newly added point
    lastAddedPoint = &points.back();
    
    return lastAddedPoint;
}

// Apply repulsion from a newly added point to all other points
void PointManager::applyRepulsion(Point* newPoint) {
    // Record the newly added point
    lastAddedPoint = newPoint;
    
    // Calculate repulsion for all points
    for (auto& p : points) {
        // Don't apply to itself
        if (&p == newPoint) continue;
        
        // Apply repulsion force
        p.applyRepulsion(*newPoint, Config::REPULSION_STRENGTH, Config::REPULSION_RADIUS);
    }
}

// Apply Brownian motion to all points
void PointManager::applyBrownianMotion() {
    for (auto& p : points) {
        p.applyBrownianMotion(Config::BROWNIAN_STRENGTH);
    }
}

// Apply constraints to keep points near their original positions
void PointManager::applyPositionConstraints() {
    for (auto& p : points) {
        p.constrainToOriginalPosition(Config::MAX_DISTANCE, Config::RETURN_FORCE);
    }
}

// Update all points' positions based on their velocities
void PointManager::updatePoints(int screenWidth, int screenHeight) {
    // Apply Brownian motion
    applyBrownianMotion();
    
    // Apply constraints to keep points near original positions
    applyPositionConstraints();
    
    for (auto& p : points) {
        // Update position
        p.update();
        
        // Constrain to screen boundaries
        p.constrainToScreen(screenWidth, screenHeight, Config::BOUNCE_FACTOR);
        
        // Apply friction
        p.applyFriction(Config::FRICTION);
    }
}
