#include "Hunter.hpp"
#include "../GameState.hpp"
#include <IO/Events/UnitAttacked.hpp>
#include <algorithm>
#include <vector>

namespace sw::game
{
    bool Hunter::canShoot(const GameState& state) const
    {
        // Hunter can't shoot if there are adjacent units
        auto adjacentUnits = state.getAdjacentUnits(_position);
        return adjacentUnits.empty();
    }

    bool Hunter::tryRangedAttack(GameState& state)
    {
        if (!canShoot(state))
        {
            return false;
        }

        // Find all units in range (from 2 to _range)
        std::vector<UnitPtr> targetsInRange;
        
        for (double r = 2.0; r <= _range; r += 0.5)
        {
            auto unitsAtRange = state.getUnitsInRange(_position, r);
            for (const auto& unit : unitsAtRange)
            {
                // Only include units that are not too close and not already in our list
                double distance = unit->getPosition().distanceTo(_position);
                if (distance >= 2.0 && 
                    std::find_if(targetsInRange.begin(), targetsInRange.end(),
                        [&unit](const UnitPtr& existing) { return existing->getId() == unit->getId(); }) 
                    == targetsInRange.end())
                {
                    targetsInRange.push_back(unit);
                }
            }
        }

        if (targetsInRange.empty())
        {
            return false;
        }

        // Choose a random target
        UnitPtr target = state.getRandomUnit(targetsInRange);
        if (!target)
        {
            return false;
        }

        // Attack the target
        auto combatTarget = std::dynamic_pointer_cast<CombatUnit>(target);
        if (!combatTarget)
        {
            return false;
        }

        combatTarget->takeDamage(_agility);
        
        // Log the attack
        state.logEvent(io::UnitAttacked{
            static_cast<uint32_t>(_id),
            static_cast<uint32_t>(target->getId()),
            static_cast<uint32_t>(_agility),
            static_cast<uint32_t>(combatTarget->getHp())
        });
        
        return true;
    }

    bool Hunter::tryMeleeAttack(GameState& state)
    {
        // Try to attack an adjacent unit
        auto adjacentUnits = state.getAdjacentUnits(_position);
        
        // Remove self from adjacent units if somehow included
        adjacentUnits.erase(
            std::remove_if(adjacentUnits.begin(), adjacentUnits.end(),
                [this](const UnitPtr& unit) { return unit->getId() == _id; }),
            adjacentUnits.end()
        );
        
        if (adjacentUnits.empty())
        {
            return false;
        }

        // Choose a random unit to attack
        UnitPtr target = state.getRandomUnit(adjacentUnits);
        if (!target)
        {
            return false;
        }

        // Attack the target
        auto combatTarget = std::dynamic_pointer_cast<CombatUnit>(target);
        if (!combatTarget)
        {
            return false;
        }

        combatTarget->takeDamage(_strength);
        
        // Log the attack
        state.logEvent(io::UnitAttacked{
            static_cast<uint32_t>(_id),
            static_cast<uint32_t>(target->getId()),
            static_cast<uint32_t>(_strength),
            static_cast<uint32_t>(combatTarget->getHp())
        });
        
        return true;
    }

    void Hunter::performAction(GameState& state)
    {
        // Try ranged attack first
        if (tryRangedAttack(state))
        {
            return; // Action completed
        }

        // If ranged attack failed, try melee attack
        if (tryMeleeAttack(state))
        {
            return; // Action completed
        }

        // If no attack was performed, try to move towards target
        if (_targetPosition)
        {
            moveTowardsTarget(state);
        }
    }
} 