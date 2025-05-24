#pragma once

#include <cmath>
#include <cstdint>

namespace sw::game
{
    struct Position
    {
        int32_t x;
        int32_t y;

        Position() : x(0), y(0) {}
        Position(int32_t x, int32_t y) : x(x), y(y) {}

        bool operator==(const Position& other) const
        {
            return x == other.x && y == other.y;
        }

        bool operator!=(const Position& other) const
        {
            return !(*this == other);
        }

        double distanceTo(const Position& other) const
        {
            int32_t dx = x - other.x;
            int32_t dy = y - other.y;
            return std::sqrt(dx * dx + dy * dy);
        }

        int32_t manhattanDistanceTo(const Position& other) const
        {
            return std::abs(x - other.x) + std::abs(y - other.y);
        }

        bool isAdjacent(const Position& other) const
        {
            return std::abs(x - other.x) <= 1 && 
                   std::abs(y - other.y) <= 1 && 
                   *this != other;
        }
    };
} 