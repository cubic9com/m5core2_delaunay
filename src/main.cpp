#include <M5Unified.h>
#include <M5GFX.h>
#include <vector>
#include "../include/Point.h"
#include "../include/PointManager.h"
#include "../include/DelaunayTriangulation.h"
#include "../include/Config.h"

// Canvas for off-screen rendering
M5Canvas canvas(&M5.Lcd);

// Points storage
std::vector<Point> points;

// Point manager and triangulation
PointManager pointManager(points);
DelaunayTriangulation triangulation(points, canvas);

// Touch state tracking
bool wasTouch = false;

void setup() {
    // Initialize M5Stack
    M5.begin();
    
    // Set speaker volume
    M5.Speaker.setVolume(Config::TONE_VOLUME);

    // Play boot sound.
    M5.Speaker.tone(Config::TONE_FREQUENCY, Config::TONE_DURATION);
    delay(100);
    M5.Speaker.tone(Config::TONE_FREQUENCY, Config::TONE_DURATION);
    delay(100);
    M5.Speaker.tone(Config::TONE_FREQUENCY, Config::TONE_DURATION);

    // Initialize canvas (same size as screen)
    canvas.createSprite(M5.Lcd.width(), M5.Lcd.height());
    
    // No need to initialize managers as they are created as global objects
    
    // Draw initial screen
    canvas.fillScreen(BLACK);
    canvas.pushSprite(0, 0);
}

void loop() {
    // Update M5Stack state
    M5.update();
    
    // Handle touch input
    bool isTouch = (M5.Touch.getCount() > 0);
    
    // Process only at the moment touch begins
    if (isTouch && !wasTouch) {
        auto touchPoint = M5.Touch.getDetail(0);
        int x = touchPoint.x;
        int y = touchPoint.y;
        
        // Play a tone as feedback
        M5.Speaker.tone(Config::TONE_FREQUENCY, Config::TONE_DURATION);

        // Add new point
        Point* newPoint = pointManager.addPoint(x, y);
        
        // Apply repulsion to the new point
        pointManager.applyRepulsion(newPoint);
        
        // Recalculate and draw Delaunay diagram
        triangulation.calculateAndDraw();
    }
    
    wasTouch = isTouch;
    
    // Update point positions
    pointManager.updatePoints(M5.Lcd.width(), M5.Lcd.height());
    
    // Recalculate and draw Delaunay triangulation
    triangulation.calculateAndDraw();
    
    // Small delay to control frame rate
    delay(10);
}
