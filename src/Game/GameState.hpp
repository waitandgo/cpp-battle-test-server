#pragma once

#include "Map.hpp"
#include "Units/Unit.hpp"
#include <IO/Events/UnitMoved.hpp>
#include <IO/Events/UnitAttacked.hpp>
#include <IO/Events/UnitDied.hpp>
#include <IO/Events/MarchEnded.hpp>
#include <IO/Events/MarchStarted.hpp>
#include <IO/Events/UnitSpawned.hpp>
#include <IO/Events/MapCreated.hpp>
#include <IO/System/EventLog.hpp>
#include <unordered_map>
#include <memory>
#include <random>
#include <vector>

namespace sw::game
{
    class GameState
    {
    private:
        Map _map;
        std::unordered_map<int32_t, UnitPtr> _units;
        uint64_t _currentTick;
        mutable std::mt19937 _randomEngine;
        sw::EventLog& _eventLog;
        static constexpr uint64_t MAX_TICKS = 1000; // Maximum number of ticks to prevent hanging

    public:
        GameState(int32_t width, int32_t height, sw::EventLog& eventLog)
            : _map(width, height), _currentTick(1), _randomEngine(std::random_device{}()), _eventLog(eventLog)
        {
            _eventLog.log(_currentTick, io::MapCreated{static_cast<uint32_t>(width), static_cast<uint32_t>(height)});
        }

        Map& getMap() { return _map; }
        const Map& getMap() const { return _map; }
        
        uint64_t getCurrentTick() const { return _currentTick; }
        void nextTick() { ++_currentTick; }

        template <typename TEvent>
        void logEvent(TEvent&& event)
        {
            _eventLog.log(_currentTick, std::forward<TEvent>(event));
        }

        bool addUnit(UnitPtr unit)
        {
            if (_units.find(unit->getId()) != _units.end())
            {
                return false; // Unit with this ID already exists
            }

            if (!_map.placeUnit(unit->getPosition(), unit->getId()))
            {
                return false; // Position is occupied or invalid
            }

            _units[unit->getId()] = unit;
            logEvent(io::UnitSpawned{
                static_cast<uint32_t>(unit->getId()), 
                unit->getType(), 
                static_cast<uint32_t>(unit->getPosition().x), 
                static_cast<uint32_t>(unit->getPosition().y)
            });
            return true;
        }

        bool removeUnit(int32_t unitId)
        {
            auto it = _units.find(unitId);
            if (it == _units.end())
            {
                return false;
            }

            _map.removeUnit(it->second->getPosition());
            _units.erase(it);
            return true;
        }

        UnitPtr getUnit(int32_t unitId) const
        {
            auto it = _units.find(unitId);
            if (it == _units.end())
            {
                return nullptr;
            }
            return it->second;
        }

        std::vector<UnitPtr> getUnitsInRange(const Position& center, double range) const
        {
            std::vector<UnitPtr> result;
            for (const auto& [id, unit] : _units)
            {
                if (unit->getPosition().distanceTo(center) <= range)
                {
                    result.push_back(unit);
                }
            }
            return result;
        }

        std::vector<UnitPtr> getAdjacentUnits(const Position& position) const
        {
            return getUnitsInRange(position, 1.5); // Slightly more than sqrt(2) to include diagonals
        }

        UnitPtr getRandomUnit(const std::vector<UnitPtr>& units) const
        {
            if (units.empty())
            {
                return nullptr;
            }
            
            std::uniform_int_distribution<size_t> dist(0, units.size() - 1);
            return units[dist(_randomEngine)];
        }

        void runSimulation()
        {
            bool hasActiveUnits = true;
            uint64_t ticksWithoutChanges = 0;
            uint64_t previousActiveCount = 0;
            
            while (hasActiveUnits && _units.size() > 1 && _currentTick < MAX_TICKS)
            {
                // Process each unit's action in order of ID
                std::vector<int32_t> unitIds;
                for (const auto& [id, _] : _units)
                {
                    unitIds.push_back(id);
                }
                
                std::sort(unitIds.begin(), unitIds.end());
                
                for (int32_t id : unitIds)
                {
                    auto unit = getUnit(id);
                    if (unit && unit->isActive())
                    {
                        unit->performAction(*this);
                    }
                }
                
                // Remove dead units
                std::vector<int32_t> deadUnits;
                for (const auto& [id, unit] : _units)
                {
                    if (!unit->isActive())
                    {
                        deadUnits.push_back(id);
                    }
                }
                
                for (int32_t id : deadUnits)
                {
                    logEvent(io::UnitDied{static_cast<uint32_t>(id)});
                    removeUnit(id);
                }
                
                // Check if we still have active units
                hasActiveUnits = false;
                uint64_t currentActiveCount = 0;
                for (const auto& [_, unit] : _units)
                {
                    if (unit->isActive())
                    {
                        hasActiveUnits = true;
                        currentActiveCount++;
                    }
                }
                
                // Check for "hanging" - if the number of active units does not change for 50 ticks,
                // then the units are probably unable to reach their targets or are in an infinite loop
                if (currentActiveCount == previousActiveCount)
                {
                    ticksWithoutChanges++;
                    if (ticksWithoutChanges >= 50)
                    {
                        break; // Exit the loop if the simulation is "stuck"
                    }
                }
                else
                {
                    ticksWithoutChanges = 0;
                    previousActiveCount = currentActiveCount;
                }
                
                // Move to next tick if we have active units
                if (hasActiveUnits && _units.size() > 1)
                {
                    nextTick();
                }
            }
        }
    };
} 