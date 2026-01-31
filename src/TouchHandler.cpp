#include "TouchHandler.h"

// Constructor
TouchHandler::TouchHandler(PointManager& pointManager)
    : wasTouch(false), pointManager(pointManager) {
}

// Update touch state and handle touch events
void TouchHandler::update() {
    // Get current touch state
    bool isTouch = (M5.Touch.getCount() > 0);
    
    // Process only at the moment touch begins
    if (isTouch && !wasTouch) {
        // Get touch coordinates
        auto touchPoint = M5.Touch.getDetail(0);
        int x = touchPoint.x;
        int y = touchPoint.y;
        
        // Play feedback sound
        playTouchFeedback();
        
        // Add new point at touch location
        Point* newPoint = pointManager.addPoint(x, y);
        
        // Apply repulsion to other points
        pointManager.applyRepulsion(newPoint);
    }
    
    // Update touch state
    wasTouch = isTouch;
}

// Play feedback sound for touch
void TouchHandler::playTouchFeedback() const {
    M5.Speaker.tone(AudioConstants::TONE_FREQUENCY, AudioConstants::TONE_DURATION);
}
