#include <stdio.h>
#include <windows.h>
#include <stdlib.h> 
#include <time.h>

int main()
{

	SetConsoleOutputCP(65001);

	int kaisuu = 5;

	unsigned int seed = 0;

	seed = (unsigned int)time(NULL);
	srand(seed);

	for (int i = 0; i < kaisuu; i++)
	{
		int dice_Deme = rand() % 6 + 1;

		printf("さいころの出目は%dです\n", dice_Deme);
	}



	return 0;
}