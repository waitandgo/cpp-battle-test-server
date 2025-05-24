#pragma once

#include "Position.hpp"
#include <vector>
#include <optional>
#include <stdexcept>

namespace sw::game
{
    class Map
    {
    private:
        int32_t _width;
        int32_t _height;
        std::vector<std::optional<int32_t>> _grid; // Stores unit IDs

    public:
        Map(int32_t width, int32_t height)
            : _width(width), _height(height), _grid(width * height, std::nullopt)
        {
            if (width <= 0 || height <= 0)
            {
                throw std::invalid_argument("Map dimensions must be positive");
            }
        }

        int32_t getWidth() const { return _width; }
        int32_t getHeight() const { return _height; }

        bool isValidPosition(const Position& pos) const
        {
            return pos.x >= 0 && pos.x < _width && pos.y >= 0 && pos.y < _height;
        }

        bool isOccupied(const Position& pos) const
        {
            if (!isValidPosition(pos))
            {
                return true; // Treat out-of-bounds as occupied
            }
            return _grid[pos.y * _width + pos.x].has_value();
        }

        std::optional<int32_t> getUnitIdAt(const Position& pos) const
        {
            if (!isValidPosition(pos))
            {
                return std::nullopt;
            }
            return _grid[pos.y * _width + pos.x];
        }

        bool placeUnit(const Position& pos, int32_t unitId)
        {
            if (!isValidPosition(pos) || isOccupied(pos))
            {
                return false;
            }
            _grid[pos.y * _width + pos.x] = unitId;
            return true;
        }

        bool removeUnit(const Position& pos)
        {
            if (!isValidPosition(pos) || !isOccupied(pos))
            {
                return false;
            }
            _grid[pos.y * _width + pos.x] = std::nullopt;
            return true;
        }

        bool moveUnit(const Position& from, const Position& to)
        {
            if (!isValidPosition(from) || !isValidPosition(to) || isOccupied(to))
            {
                return false;
            }

            auto unitId = getUnitIdAt(from);
            if (!unitId)
            {
                return false;
            }

            _grid[from.y * _width + from.x] = std::nullopt;
            _grid[to.y * _width + to.x] = unitId;
            return true;
        }

        std::vector<Position> getAdjacentPositions(const Position& pos) const
        {
            std::vector<Position> result;
            for (int32_t dy = -1; dy <= 1; ++dy)
            {
                for (int32_t dx = -1; dx <= 1; ++dx)
                {
                    if (dx == 0 && dy == 0)
                    {
                        continue;
                    }
                    
                    Position adjacent(pos.x + dx, pos.y + dy);
                    if (isValidPosition(adjacent))
                    {
                        result.push_back(adjacent);
                    }
                }
            }
            return result;
        }
    };
} 