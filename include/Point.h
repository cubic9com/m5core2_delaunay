#pragma once

#include <Arduino.h>

// Point class representing a point in 2D space with velocity
class Point {
public:
    float x;
    float y;
    float vx;        // velocity in x direction
    float vy;        // velocity in y direction
    float origX;     // original position X
    float origY;     // original position Y

    // Constructor
    Point(float _x, float _y) 
        : x(_x), y(_y), vx(0), vy(0), origX(_x), origY(_y) {}

    // Calculate squared distance to another point
    float distanceSquaredTo(const Point& other) const {
        return (x - other.x) * (x - other.x) + (y - other.y) * (y - other.y);
    }

    // Calculate distance to another point
    float distanceTo(const Point& other) const {
        return sqrt(distanceSquaredTo(other));
    }

    // Apply friction to velocity
    void applyFriction(float friction) {
        vx *= friction;
        vy *= friction;
    }

    // Apply force in direction of another point
    void applyForce(const Point& other, float force) {
        float dx = other.x - x;
        float dy = other.y - y;
        float dist = sqrt(dx * dx + dy * dy);
        
        if (dist > 0) {
            vx += dx / dist * force;
            vy += dy / dist * force;
        }
    }

    // Apply repulsion force away from another point
    void applyRepulsion(const Point& other, float strength, float radius) {
        float dx = x - other.x;
        float dy = y - other.y;
        float distSq = dx * dx + dy * dy;
        
        if (distSq < radius * radius && distSq > 0) {
            float dist = sqrt(distSq);
            float force = strength * (1.0 - dist / radius);
            
            vx += dx / dist * force;
            vy += dy / dist * force;
        }
    }

    // Apply random Brownian motion
    void applyBrownianMotion(float strength) {
        vx += (random(1000) / 1000.0 - 0.5) * strength;
        vy += (random(1000) / 1000.0 - 0.5) * strength;
    }

    // Apply force to return to original position if too far
    void constrainToOriginalPosition(float maxDistance, float returnForce) {
        float dx = x - origX;
        float dy = y - origY;
        float dist = sqrt(dx * dx + dy * dy);
        
        if (dist > maxDistance) {
            float force = (dist - maxDistance) * returnForce;
            vx -= dx / dist * force;
            vy -= dy / dist * force;
        }
    }

    // Update position based on velocity
    void update() {
        x += vx;
        y += vy;
    }

    // Constrain to screen boundaries
    void constrainToScreen(int width, int height, float bounceFactor = 0.5) {
        if (x < 0) { x = 0; vx = -vx * bounceFactor; }
        if (x > width) { x = width; vx = -vx * bounceFactor; }
        if (y < 0) { y = 0; vy = -vy * bounceFactor; }
        if (y > height) { y = height; vy = -vy * bounceFactor; }
    }
};
