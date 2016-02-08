#include "team.h"
#include <stdio.h>
#include <Windows.h>
#include <Psapi.h>
#include <time.h>

SIZE_T GetProcessWorkingSetSizeS()
{
	PROCESS_MEMORY_COUNTERS pmc;
	if (!GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc)))
	{
		return 0;
	}
	return pmc.WorkingSetSize;
}

void main()
{
	int i;

	SIZE_T usage;
	char *keyword[3] = { "This", "From", "beauty" };
	clock_t start, end;

	start = clock();

	usage = GetProcessWorkingSetSizeS();

	initialize();
	read_from_file("data.txt");

	for (i = 0; i < 3; ++i){
		search_and_print(keyword[i], "student.txt");
	}

	usage = GetProcessWorkingSetSizeS() - usage;

	end = clock();

	printf("\n");
	printf("실행시간 : %f 초 \n", (double)(end - start) / CLOCKS_PER_SEC);
	printf("%ld byte \n", usage);
}