#pragma once

#include <Arduino.h>

// Configuration constants for the application
namespace Config {
    // Display settings
    constexpr int POINT_RADIUS = 5;
    constexpr int LINE_THICKNESS = 2;
    
    // Audio feedback settings
    constexpr uint8_t TONE_VOLUME = 48U;
    constexpr float TONE_FREQUENCY = 659.26F;
    constexpr uint32_t TONE_DURATION = 50U;
    
    // Point behavior settings
    constexpr int MAX_POINTS = 30;
    constexpr float FRICTION = 0.9F;
    
    // Brownian motion settings
    constexpr float BROWNIAN_STRENGTH = 0.2F;
    constexpr float MAX_DISTANCE = 5.0F;
    constexpr float RETURN_FORCE = 0.1F;
    
    // Repulsion settings
    constexpr float REPULSION_RADIUS = 100.0F;
    constexpr float REPULSION_STRENGTH = 5.0F;
    
    // Screen edge bounce settings
    constexpr float BOUNCE_FACTOR = 0.5F;
}
