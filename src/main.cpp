#include <M5Unified.h>
#include <M5GFX.h>
#include <vector>
#include "Constants.h"
#include "Point.h"
#include "PointManager.h"
#include "DelaunayTriangulation.h"
#include "TouchHandler.h"

// Canvas for off-screen rendering
M5Canvas canvas(&M5.Lcd);

// Points storage
std::vector<Point> points;

// Point manager, triangulation and touch handler
PointManager pointManager(points);
DelaunayTriangulation triangulation(points, canvas);
TouchHandler touchHandler(pointManager);

void setup() {
    // Initialize M5Stack
    M5.begin();
    
    // Set speaker volume
    M5.Speaker.setVolume(AudioConstants::TONE_VOLUME);

    // Play boot sound.
    M5.Speaker.tone(AudioConstants::TONE_FREQUENCY, AudioConstants::TONE_DURATION);
    delay(100);
    M5.Speaker.tone(AudioConstants::TONE_FREQUENCY, AudioConstants::TONE_DURATION);
    delay(100);
    M5.Speaker.tone(AudioConstants::TONE_FREQUENCY, AudioConstants::TONE_DURATION);

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
    bool touchProcessed = touchHandler.update();
    
    // If touch was processed, redraw the triangulation
    if (touchProcessed) {
        triangulation.calculateAndDraw();
    }
    
    // Update point positions
    pointManager.updatePoints(M5.Lcd.width(), M5.Lcd.height());
    
    // Recalculate and draw Delaunay triangulation
    triangulation.calculateAndDraw();
    
    // Small delay to control frame rate
    delay(10);
}
