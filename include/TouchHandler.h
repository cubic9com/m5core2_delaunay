#pragma once

#include <M5Unified.h>
#include "Constants.h"
#include "Point.h"
#include "PointManager.h"

// Class for handling touch input
class TouchHandler {
private:
    // Previous touch state
    bool wasTouch;
    
    // Reference to the point manager
    PointManager& pointManager;

public:
    // Constructor
    TouchHandler(PointManager& pointManager);
    
    // Update touch state and handle touch events
    // Returns true if a touch event was processed
    bool update();
    
    // Get the current touch state
    bool isTouching() const { return wasTouch; }
    
    // Play feedback sound for touch
    void playTouchFeedback() const;
};
