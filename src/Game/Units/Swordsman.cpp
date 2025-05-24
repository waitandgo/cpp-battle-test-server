#include "Swordsman.hpp"
#include "../GameState.hpp"
#include <IO/Events/UnitAttacked.hpp>

namespace sw::game
{
    void Swordsman::performAction(GameState& state)
    {
        // First, try to attack an adjacent unit
        auto adjacentUnits = state.getAdjacentUnits(_position);
        
        // Remove self from adjacent units if somehow included
        adjacentUnits.erase(
            std::remove_if(adjacentUnits.begin(), adjacentUnits.end(),
                [this](const UnitPtr& unit) { return unit->getId() == _id; }),
            adjacentUnits.end()
        );
        
        if (!adjacentUnits.empty())
        {
            // Choose a random unit to attack
            UnitPtr target = state.getRandomUnit(adjacentUnits);
            
            if (target)
            {
                // Attack the target
                auto combatTarget = std::dynamic_pointer_cast<CombatUnit>(target);
                if (combatTarget)
                {
                    combatTarget->takeDamage(_strength);
                    
                    // Log the attack
                    state.logEvent(io::UnitAttacked{
                        static_cast<uint32_t>(_id),
                        static_cast<uint32_t>(target->getId()),
                        static_cast<uint32_t>(_strength),
                        static_cast<uint32_t>(combatTarget->getHp())
                    });
                    
                    return; // Action completed
                }
            }
        }
        
        // If no attack was performed, try to move towards target
        if (_targetPosition)
        {
            moveTowardsTarget(state);
        }
    }
} 