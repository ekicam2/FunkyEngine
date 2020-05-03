#if _DEBUG

#define FK_IMPLEMENTATION
#include "fk.h"

#define TEST_GREETING() printf("Welcome to fk library test environment! \n\n");
#define TEST_FK_PLATFORM() printf("At first we will be testing OS specyfic functions \n");
#define TEST_FK_STRING() printf("Now we will focus on our string functionality \n");

#include <cstdio>


#define TEST_FUNC(var, func)	\
	printf("testing function: %s \n", #func); \
	auto var = func; 

#endif

#ifdef main
#undef main
#endif

int main()
{
	TEST_GREETING();

	TEST_FK_PLATFORM();

	TEST_FUNC(path, fk::getExecPath())
		wprintf(L"\tpath: %s \n", path);
	delete[] path;

	TEST_FK_STRING();
	fk::str str1 = L" lolloolofloflo";
	{
		fk::str str2 = str1;
		if (str1 == str2)
			printf("correctly passed comparasion string test");
	}

	fk::str str2 = str1;
	str1 = L"s22431dsdasd";
	
	return 0;
}