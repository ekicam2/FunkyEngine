#include "FunkyEngine.h"

#include "Rendering/RawMesh.h"
#include "Rendering/Camera.h"
#include "Rendering/Light.h"
#include "Rendering/Texture.h"

#include <fstream>
#include <iterator>
#include <vector>
#include <locale>

#ifdef MEMORY_TEST

#include "Core/Memory/MemoryPool.h"


struct Tester10MB
{
	byte __unused[10 * 1024 * 1024];
};
static_assert(sizeof(Tester10MB) == 10 * 1024 * 1024);


void DebugMemoryPool()
{
	BREAK();

	Funky::Core::Memory::MemoryPool MP;
	if (!MP.InitMemoryPool(512 * 1024 * 1024))
	BREAK();
	BREAK();


	Tester10MB* pTester = MP.Alloc<Tester10MB>();
	(void)pTester;
	BREAK();


	i32 allocations = std::rand() % 100;
	std::vector<void*> AllocatedDumbs;
	for (i32 i = 0; i < allocations; ++i)
	{
		AllocatedDumbs.push_back((void*)MP.Alloc<Tester10MB>());
	}

	do
	{
		i32 randomID = std::rand() % AllocatedDumbs.size();
		MP.Free(AllocatedDumbs[randomID]);
	} while (AllocatedDumbs.size() == 0);

	MP.Free(pTester);
	BREAK();

}

int main()
{
	// Something is wrong
	// im not able to deallocate all memory ive allocated!
	DebugMemoryPool();

	return 0;
	
}
#else 
int main()
{
	Funky::FunkyEngine Engine;
	if (Engine.Init())
		Engine.Run();

	Engine.Shutdown();

	return 0;
}
#endif