#pragma once

#include "GameState.hpp"
#include "Units/Swordsman.hpp"
#include "Units/Hunter.hpp"
#include <IO/Commands/CreateMap.hpp>
#include <IO/Commands/SpawnSwordsman.hpp>
#include <IO/Commands/SpawnHunter.hpp>
#include <IO/Commands/March.hpp>
#include <IO/Events/MarchStarted.hpp>
#include <memory>

namespace sw::game
{
    class GameController
    {
    private:
        std::unique_ptr<GameState> _gameState;
        sw::EventLog& _eventLog;
        bool _isInitialized;

    public:
        GameController(sw::EventLog& eventLog)
            : _gameState(nullptr), _eventLog(eventLog), _isInitialized(false) {}

        void handleCreateMap(const io::CreateMap& command)
        {
            _gameState = std::make_unique<GameState>(
                static_cast<int32_t>(command.width),
                static_cast<int32_t>(command.height),
                _eventLog
            );
            _isInitialized = true;
        }

        void handleSpawnSwordsman(const io::SpawnSwordsman& command)
        {
            if (!_isInitialized)
            {
                throw std::runtime_error("Game not initialized. Create a map first.");
            }

            auto swordsman = std::make_shared<Swordsman>(
                static_cast<int32_t>(command.unitId),
                Position(static_cast<int32_t>(command.x), static_cast<int32_t>(command.y)),
                static_cast<int32_t>(command.hp),
                static_cast<int32_t>(command.strength)
            );

            if (!_gameState->addUnit(swordsman))
            {
                throw std::runtime_error("Failed to spawn swordsman. Position might be occupied or invalid.");
            }
        }

        void handleSpawnHunter(const io::SpawnHunter& command)
        {
            if (!_isInitialized)
            {
                throw std::runtime_error("Game not initialized. Create a map first.");
            }

            auto hunter = std::make_shared<Hunter>(
                static_cast<int32_t>(command.unitId),
                Position(static_cast<int32_t>(command.x), static_cast<int32_t>(command.y)),
                static_cast<int32_t>(command.hp),
                static_cast<int32_t>(command.agility),
                static_cast<int32_t>(command.strength),
                static_cast<int32_t>(command.range)
            );

            if (!_gameState->addUnit(hunter))
            {
                throw std::runtime_error("Failed to spawn hunter. Position might be occupied or invalid.");
            }
        }

        void handleMarch(const io::March& command)
        {
            if (!_isInitialized)
            {
                throw std::runtime_error("Game not initialized. Create a map first.");
            }

            auto unit = _gameState->getUnit(static_cast<int32_t>(command.unitId));
            if (!unit)
            {
                throw std::runtime_error("Unit not found.");
            }

            Position target(static_cast<int32_t>(command.targetX), static_cast<int32_t>(command.targetY));
            unit->setTargetPosition(target);

            // Log march started event
            _gameState->logEvent(io::MarchStarted{
                command.unitId,
                static_cast<uint32_t>(unit->getPosition().x),
                static_cast<uint32_t>(unit->getPosition().y),
                command.targetX,
                command.targetY
            });
        }

        void runSimulation()
        {
            if (!_isInitialized)
            {
                throw std::runtime_error("Game not initialized. Create a map first.");
            }

            _gameState->runSimulation();
        }
    };
} 