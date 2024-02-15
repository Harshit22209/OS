#include <stdio.h>
#include <unistd.h>
int main()
{
	// printf("Hello world");
	// sleep(100);
	long cnt = 0;
	printf("i am child\n");
	while (cnt < 1000000)
	{
	 	cnt++;
	 	long cnt2 = 0;
	 	while (cnt2 < 10000)
	 	{
	 		cnt2++;
	 		//long cnt3 = 0;
	 		//while (cnt3 < 1000)
	 		//{
	 		//	cnt3++;
	 		//}
	 	}
	 }
}
