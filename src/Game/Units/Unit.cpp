#include "Unit.hpp"
#include "../GameState.hpp"
#include <IO/Events/UnitMoved.hpp>
#include <IO/Events/MarchEnded.hpp>
#include <algorithm>
#include <vector>

namespace sw::game
{
    bool Unit::moveTowardsTarget(GameState& state)
    {
        if (!_targetPosition)
        {
            return false;
        }

        // If already at target position, we're done
        if (_position == *_targetPosition)
        {
            _targetPosition = std::nullopt;
            return false;
        }

        // Get all adjacent positions
        auto adjacentPositions = state.getMap().getAdjacentPositions(_position);
        
        // Filter out occupied positions
        adjacentPositions.erase(
            std::remove_if(adjacentPositions.begin(), adjacentPositions.end(),
                [&state](const Position& pos) { return state.getMap().isOccupied(pos); }),
            adjacentPositions.end()
        );

        if (adjacentPositions.empty())
        {
            return false; // No valid moves
        }

        // Find the position closest to the target
        Position bestMove = adjacentPositions[0];
        double bestDistance = bestMove.distanceTo(*_targetPosition);

        for (size_t i = 1; i < adjacentPositions.size(); ++i)
        {
            double distance = adjacentPositions[i].distanceTo(*_targetPosition);
            if (distance < bestDistance)
            {
                bestMove = adjacentPositions[i];
                bestDistance = distance;
            }
        }

        // Move to the best position
        Position oldPosition = _position;
        _position = bestMove;
        
        // Update the map
        state.getMap().moveUnit(oldPosition, bestMove);
        
        // Log the movement event
        state.logEvent(io::UnitMoved{
            static_cast<uint32_t>(_id), 
            static_cast<uint32_t>(bestMove.x), 
            static_cast<uint32_t>(bestMove.y)
        });

        // Check if we've reached the target
        if (_position == *_targetPosition)
        {
            state.logEvent(io::MarchEnded{
                static_cast<uint32_t>(_id), 
                static_cast<uint32_t>(_position.x), 
                static_cast<uint32_t>(_position.y)
            });
            _targetPosition = std::nullopt;
        }

        return true;
    }
} 