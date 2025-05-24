#pragma once

#include "CombatUnit.hpp"

namespace sw::game
{
    class Hunter : public CombatUnit
    {
    private:
        int32_t _agility;
        int32_t _strength;
        int32_t _range;

    public:
        Hunter(int32_t id, const Position& position, int32_t hp, int32_t agility, int32_t strength, int32_t range)
            : CombatUnit(id, position, "Hunter", hp), _agility(agility), _strength(strength), _range(range) {}

        int32_t getAgility() const { return _agility; }
        int32_t getStrength() const { return _strength; }
        int32_t getRange() const { return _range; }

        void performAction(GameState& state) override;
        
    private:
        bool canShoot(const GameState& state) const;
        bool tryRangedAttack(GameState& state);
        bool tryMeleeAttack(GameState& state);
    };
} 