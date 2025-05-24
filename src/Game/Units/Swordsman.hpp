#pragma once

#include "CombatUnit.hpp"

namespace sw::game
{
    class Swordsman : public CombatUnit
    {
    private:
        int32_t _strength;

    public:
        Swordsman(int32_t id, const Position& position, int32_t hp, int32_t strength)
            : CombatUnit(id, position, "Swordsman", hp), _strength(strength) {}

        int32_t getStrength() const { return _strength; }

        void performAction(GameState& state) override;
    };
} 