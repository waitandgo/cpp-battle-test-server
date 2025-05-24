#include <IO/Commands/CreateMap.hpp>
#include <IO/Commands/March.hpp>
#include <IO/Commands/SpawnHunter.hpp>
#include <IO/Commands/SpawnSwordsman.hpp>
#include <IO/Events/MapCreated.hpp>
#include <IO/Events/MarchEnded.hpp>
#include <IO/Events/MarchStarted.hpp>
#include <IO/Events/UnitAttacked.hpp>
#include <IO/Events/UnitDied.hpp>
#include <IO/Events/UnitMoved.hpp>
#include <IO/Events/UnitSpawned.hpp>
#include <IO/System/CommandParser.hpp>
#include <IO/System/EventLog.hpp>
#include <IO/System/PrintDebug.hpp>
#include <Game/GameController.hpp>
#include <fstream>
#include <iostream>

int main(int argc, char** argv)
{
	using namespace sw;

	if (argc != 2)
	{
		throw std::runtime_error("Error: No file specified in command line argument");
	}

	std::ifstream file(argv[1]);
	if (!file)
	{
		throw std::runtime_error("Error: File not found - " + std::string(argv[1]));
	}

	std::cout << "Commands:\n";
	
	EventLog eventLog;
	game::GameController gameController(eventLog);
	
	io::CommandParser parser;
	parser.add<io::CreateMap>([&gameController](auto command) { 
		printDebug(std::cout, command); 
		gameController.handleCreateMap(command);
	})
	.add<io::SpawnSwordsman>([&gameController](auto command) { 
		printDebug(std::cout, command); 
		gameController.handleSpawnSwordsman(command);
	})
	.add<io::SpawnHunter>([&gameController](auto command) { 
		printDebug(std::cout, command); 
		gameController.handleSpawnHunter(command);
	})
	.add<io::March>([&gameController](auto command) { 
		printDebug(std::cout, command); 
		gameController.handleMarch(command);
	});

	parser.parse(file);

	std::cout << "\n\nEvents:\n";
	
	// Run simulation after all commands are processed
	gameController.runSimulation();

	std::cout << "\n\nSimulation ended\n";

	return 0;
}
