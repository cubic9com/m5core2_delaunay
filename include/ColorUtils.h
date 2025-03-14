#pragma once

#include <Arduino.h>

// Utility functions for color generation and manipulation
namespace ColorUtils {
    // Generate a pastel color from a seed value
    inline uint16_t generatePastelColor(uint32_t seed) {
        // Generate pseudo-random numbers using seed
        uint8_t r = ((seed * 1664525 + 1013904223) % 128) + 128; // range 128-255
        uint8_t g = ((seed * 22695477 + 1) % 128) + 128; // range 128-255
        uint8_t b = ((seed * 214013 + 2531011) % 128) + 128; // range 128-255
        
        // Convert to RGB565 format (format used by M5Stack)
        return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
    }
}
