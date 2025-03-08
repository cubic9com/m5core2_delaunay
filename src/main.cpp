#include <M5Unified.h>
#include <M5GFX.h>
#include <vector>

// Canvas for off-screen rendering
M5Canvas canvas(&M5.Lcd);

// Point structure
struct Point {
    float x;
    float y;
    float vx = 0;  // velocity in x direction
    float vy = 0;  // velocity in y direction
    float origX;   // original position X
    float origY;   // original position Y
    Point(int _x, int _y) : x(_x), y(_y), origX(_x), origY(_y) {}
};

// Brownian motion settings
const float BROWNIAN_STRENGTH = 0.2;  // strength of Brownian motion
const float MAX_DISTANCE = 5.0;       // maximum distance from original position

// Repulsion settings
const float REPULSION_RADIUS = 100.0;  // radius of repulsion effect
const float REPULSION_STRENGTH = 5.0;  // strength of repulsion
const float FRICTION = 0.9;            // friction coefficient (damping)

// Function to generate pastel colors
uint16_t generatePastelColor(uint32_t seed) {
    // Generate pseudo-random numbers using seed
    uint8_t r = ((seed * 1664525 + 1013904223) % 128) + 128; // range 128-255
    uint8_t g = ((seed * 22695477 + 1) % 128) + 128; // range 128-255
    uint8_t b = ((seed * 214013 + 2531011) % 128) + 128; // range 128-255
    
    // Convert to RGB565 format (format used by M5Stack)
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

// Triangle structure
struct Triangle {
    Point *p1, *p2, *p3;
    Triangle(Point* _p1, Point* _p2, Point* _p3) : p1(_p1), p2(_p2), p3(_p3) {}
};

// Array of points
std::vector<Point> points;

// Maximum number of points
const int MAX_POINTS = 50;

// Previous touch state
bool wasTouch = false;

// Last added point
Point* lastAddedPoint = nullptr;

// Function to apply repulsion
void applyRepulsion(Point* newPoint) {
    // Record the newly added point
    lastAddedPoint = newPoint;
    
    // Calculate repulsion for all points
    for (auto& p : points) {
        // Don't apply to itself
        if (&p == newPoint) continue;
        
        // Calculate distance between two points
        float dx = p.x - newPoint->x;
        float dy = p.y - newPoint->y;
        float distSq = dx * dx + dy * dy;
        
        // Apply repulsion if within radius
        if (distSq < REPULSION_RADIUS * REPULSION_RADIUS && distSq > 0) {
            float dist = sqrt(distSq);
            float force = REPULSION_STRENGTH * (1.0 - dist / REPULSION_RADIUS);
            
            // Convert to unit vector and apply force
            float fx = dx / dist * force;
            float fy = dy / dist * force;
            
            // Add to velocity
            p.vx += fx;
            p.vy += fy;
        }
    }
}

// Calculate squared distance between two points
float distSquared(const Point& p1, const Point& p2) {
    return (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y);
}

// Check if point p is inside the circumcircle of triangle t
bool isPointInCircumcircle(const Triangle& t, const Point& p) {
    // Three points of the triangle
    float x1 = t.p1->x;
    float y1 = t.p1->y;
    float x2 = t.p2->x;
    float y2 = t.p2->y;
    float x3 = t.p3->x;
    float y3 = t.p3->y;
    
    // Calculate determinant
    float a = x1 * (y2 - y3) - y1 * (x2 - x3) + x2 * y3 - x3 * y2;
    float b = (x1 * x1 + y1 * y1) * (y3 - y2) + (x2 * x2 + y2 * y2) * (y1 - y3) + (x3 * x3 + y3 * y3) * (y2 - y1);
    float c = (x1 * x1 + y1 * y1) * (x2 - x3) + (x2 * x2 + y2 * y2) * (x3 - x1) + (x3 * x3 + y3 * y3) * (x1 - x2);
    float d = (x1 * x1 + y1 * y1) * (x3 * y2 - x2 * y3) + (x2 * x2 + y2 * y2) * (x1 * y3 - x3 * y1) + (x3 * x3 + y3 * y3) * (x2 * y1 - x1 * y2);
    
    // Center and squared radius of circumcircle
    float centerX = -b / (2 * a);
    float centerY = -c / (2 * a);
    float radiusSquared = (b * b + c * c - 4 * a * d) / (4 * a * a);
    
    // Squared distance between point and center
    float distSq = (p.x - centerX) * (p.x - centerX) + (p.y - centerY) * (p.y - centerY);
    
    // Inside circle if distance is less than radius
    return distSq < radiusSquared;
}

// Calculate and draw Delaunay triangulation
void drawDelaunayTriangulation() {
    // Clear canvas
    canvas.fillScreen(BLACK);
    
    // Can't make triangles with less than 3 points
    if (points.size() < 3) {
        // Draw points (white)
        for (const auto& p : points) {
            canvas.fillCircle(p.x, p.y, 5, WHITE);
        }
        
        // Transfer canvas content to display
        canvas.pushSprite(0, 0);
        return;
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
                        if (isPointInCircumcircle(tri, points[l])) {
                            isDelaunay = false;
                            break;
                        }
                    }
                }
                
                // Draw triangle if it satisfies Delaunay condition
                if (isDelaunay) {
                    // Generate fixed color for each triangle (using vertex indices)
                    uint32_t colorSeed = i * 10000 + j * 100 + k;
                    uint16_t triangleColor = generatePastelColor(colorSeed);
                    
                    // Draw each line twice to make 2-pixel thick lines
                    canvas.drawLine(tri.p1->x, tri.p1->y, tri.p2->x, tri.p2->y, triangleColor);
                    canvas.drawLine(tri.p1->x+1, tri.p1->y, tri.p2->x+1, tri.p2->y, triangleColor);
                    
                    canvas.drawLine(tri.p2->x, tri.p2->y, tri.p3->x, tri.p3->y, triangleColor);
                    canvas.drawLine(tri.p2->x+1, tri.p2->y, tri.p3->x+1, tri.p3->y, triangleColor);
                    
                    canvas.drawLine(tri.p3->x, tri.p3->y, tri.p1->x, tri.p1->y, triangleColor);
                    canvas.drawLine(tri.p3->x+1, tri.p3->y, tri.p1->x+1, tri.p1->y, triangleColor);
                }
            }
        }
    }
    
    // Draw points (white) - after triangles so they appear on top of lines
    for (const auto& p : points) {
        canvas.fillCircle(p.x, p.y, 5, WHITE);
    }
    
    // Transfer canvas content to display
    canvas.pushSprite(0, 0);
}

// Function to apply Brownian motion
void applyBrownianMotion() {
    for (auto& p : points) {
        // Add small random velocity in random direction
        p.vx += (random(1000) / 1000.0 - 0.5) * BROWNIAN_STRENGTH;
        p.vy += (random(1000) / 1000.0 - 0.5) * BROWNIAN_STRENGTH;
        
        // Calculate distance from original position
        float dx = p.x - p.origX;
        float dy = p.y - p.origY;
        float dist = sqrt(dx * dx + dy * dy);
        
        // If beyond max distance, add force to return to original position
        if (dist > MAX_DISTANCE) {
            float force = (dist - MAX_DISTANCE) * 0.1;
            p.vx -= dx / dist * force;
            p.vy -= dy / dist * force;
        }
    }
}

// Function to update point positions
void updatePoints() {
    bool moved = false;
    
    // Apply Brownian motion
    applyBrownianMotion();
    
    for (auto& p : points) {
        // Update position
        p.x += p.vx;
        p.y += p.vy;
        
        // Bounce at screen edges
        if (p.x < 0) { p.x = 0; p.vx = -p.vx * 0.5; }
        if (p.x > M5.Lcd.width()) { p.x = M5.Lcd.width(); p.vx = -p.vx * 0.5; }
        if (p.y < 0) { p.y = 0; p.vy = -p.vy * 0.5; }
        if (p.y > M5.Lcd.height()) { p.y = M5.Lcd.height(); p.vy = -p.vy * 0.5; }
        
        // Damping due to friction
        p.vx *= FRICTION;
        p.vy *= FRICTION;
        
        moved = true;
    }
    
    // Always redraw (for Brownian motion)
    drawDelaunayTriangulation();
}

void setup() {
    M5.begin();
    
    // Initialize canvas (same size as screen)
    canvas.createSprite(M5.Lcd.width(), M5.Lcd.height());
    
    // Draw initial screen
    canvas.fillScreen(BLACK);
    
    canvas.setTextColor(WHITE);
    canvas.setTextSize(2);
    canvas.setCursor(10, 50);
    canvas.println("Delaunay Diagram");
    canvas.setCursor(10, 80);
    canvas.println("Touch to add points");
    
    // Transfer canvas content to display
    canvas.pushSprite(0, 0);
}

void loop() {
    M5.update();  // Update M5Stack state
    
    // Get touch events
    bool isTouch = (M5.Touch.getCount() > 0);
    
    // Process only at the moment touch begins
    if (isTouch && !wasTouch) {
        auto touchPoint = M5.Touch.getDetail(0);
        int x = touchPoint.x;
        int y = touchPoint.y;
        
        // Don't exceed maximum number of points
        if (points.size() < MAX_POINTS) {
            // Add new point
            points.emplace_back(x, y);
            
            // Apply repulsion to the new point
            applyRepulsion(&points.back());
            
            // Recalculate and draw Delaunay diagram
            drawDelaunayTriangulation();
        }
    }
    
    wasTouch = isTouch;
    
    // Update point positions
    updatePoints();
    
    delay(10); // Loop interval
}
