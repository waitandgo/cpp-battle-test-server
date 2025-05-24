#pragma once

#include "../Position.hpp"
#include <string>
#include <memory>
#include <optional>

namespace sw::game
{
    class GameState;

    class Unit
    {
    protected:
        int32_t _id;
        Position _position;
        std::optional<Position> _targetPosition;
        std::string _type;

    public:
        Unit(int32_t id, const Position& position, const std::string& type)
            : _id(id), _position(position), _targetPosition(std::nullopt), _type(type) {}
        
        virtual ~Unit() = default;

        int32_t getId() const { return _id; }
        const Position& getPosition() const { return _position; }
        const std::string& getType() const { return _type; }
        
        void setPosition(const Position& position) { _position = position; }
        
        std::optional<Position> getTargetPosition() const { return _targetPosition; }
        void setTargetPosition(const Position& target) { _targetPosition = target; }
        void clearTargetPosition() { _targetPosition = std::nullopt; }

        // Each unit type will implement its own action logic
        virtual void performAction(GameState& state) = 0;

        // Check if unit is alive and can perform actions
        virtual bool isActive() const = 0;

        // Move towards target if one exists
        virtual bool moveTowardsTarget(GameState& state);
    };

    using UnitPtr = std::shared_ptr<Unit>;
} 