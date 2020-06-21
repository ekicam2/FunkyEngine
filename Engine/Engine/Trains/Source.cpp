#include "Macros.h"
#include "Engine.h"

#define INIT_LOG(...) 								\
HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);	\
DWORD consoleMode;									\
GetConsoleMode(console, &consoleMode);				\
consoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;	\
SetConsoleMode(console, consoleMode);					

int main(i32 argc, char** argv)
{
	INIT_LOG();

	Funky::Engine::EngineInitDesc EngineInit = {};
	EngineInit.Argc = argc;
	EngineInit.Argv = argv;
	EngineInit.RequiredSubsystems = Funky::SubsystemBitmask::All;
	EngineInit.Initializer.RegisterLambda([](Funky::Engine* engine, bool bWasSuccessfull) {

	});

	{
		Funky::Engine Engine;
		if (Engine.Init(&EngineInit))
			Engine.Run();

		if (!Engine.Shutdown())
			LOG_ERROR("Error while cleaning up.");
	}

	return 0;
}
