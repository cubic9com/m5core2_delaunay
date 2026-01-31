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

    // Initialize display and optimize settings
    M5.Display.init();
    M5.Display.setColorDepth(8);  // Set to 8-bit color depth

    // Set speaker volume
    M5.Speaker.setVolume(AudioConstants::TONE_VOLUME);

    // Play boot sound.
    M5.Speaker.tone(AudioConstants::TONE_FREQUENCY, AudioConstants::TONE_DURATION);
    delay(100);
    M5.Speaker.tone(AudioConstants::TONE_FREQUENCY, AudioConstants::TONE_DURATION);
    delay(100);
    M5.Speaker.tone(AudioConstants::TONE_FREQUENCY, AudioConstants::TONE_DURATION);

    // Initialize canvas (same size as screen)
    canvas.setColorDepth(8);  // Set to 8-bit color depth
    canvas.createSprite(M5.Display.width(), M5.Display.height());
    
    // Start drawing (continuous drawing mode)
    M5.Display.startWrite();
    
    // Draw initial screen
    canvas.fillScreen(BLACK);
    canvas.pushSprite(0, 0);
}

void loop() {
    // Update M5Stack state
    M5.update();
    
    // Handle touch input
    touchHandler.update();
    
    // Update point positions
    pointManager.updatePoints(M5.Display.width(), M5.Display.height());
    
    // Recalculate and draw Delaunay triangulation (only once per frame)
    triangulation.calculateAndDraw();
    
    // Small delay to control frame rate (16ms ≈ 60fps)
    delay(16);
}
