#pragma once

#include "Unit.hpp"

namespace sw::game
{
    class CombatUnit : public Unit
    {
    protected:
        int32_t _hp;
        int32_t _maxHp;

    public:
        CombatUnit(int32_t id, const Position& position, const std::string& type, int32_t hp)
            : Unit(id, position, type), _hp(hp), _maxHp(hp) {}

        int32_t getHp() const { return _hp; }
        int32_t getMaxHp() const { return _maxHp; }
        
        void takeDamage(int32_t amount)
        {
            _hp = std::max(0, _hp - amount);
        }
        
        void heal(int32_t amount)
        {
            _hp = std::min(_maxHp, _hp + amount);
        }
        
        bool isActive() const override
        {
            return _hp > 0;
        }
    };
} 