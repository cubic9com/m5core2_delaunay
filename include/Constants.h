#pragma once

#include <Arduino.h>

// Display related constants
namespace DisplayConstants {
    constexpr int POINT_RADIUS = 5;
    constexpr int LINE_THICKNESS = 2;
}

// Audio feedback related constants
namespace AudioConstants {
    constexpr uint8_t TONE_VOLUME = 48U;
    constexpr float TONE_FREQUENCY = 659.26F;
    constexpr uint32_t TONE_DURATION = 50U;
}

// Point behavior related constants
namespace PointConstants {
    constexpr int MAX_POINTS = 30;
}

// Physics simulation related constants
namespace PhysicsConstants {
    constexpr float FRICTION = 0.9F;
    constexpr float BOUNCE_FACTOR = 0.5F;
}

// Brownian motion related constants
namespace BrownianConstants {
    constexpr float STRENGTH = 0.2F;
    constexpr float MAX_DISTANCE = 5.0F;
    constexpr float RETURN_FORCE = 0.1F;
}

// Repulsion force related constants
namespace RepulsionConstants {
    constexpr float RADIUS = 100.0F;
    constexpr float STRENGTH = 5.0F;
}
