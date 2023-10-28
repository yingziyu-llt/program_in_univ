/*
 * Copyright (C) 2023 Letian Lin <yingziyu-Lin@outlook.com>
 *
 * Distributed under terms of the GNU AGPLv3+ license.
 * 
 * Run for our better future
 */

#include<stdio.h>
#include<algorithm>
#include<string.h>
#include<math.h>
using namespace std;


bool is_run(int t)
{
	return ((t % 400 == 0) || (t % 100 && t % 4 == 0));
}

int main()
{
#ifdef llt
	freopen("8.in","r",stdin);
	freopen("8.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	for(int i = 1;i <= n;i++)
	{
		int t;
		scanf("%d",&t);
		if(t == 0)
		{
			printf("2\n");
			continue;
		}
		if(t == 1)
		{
			printf("4\n");
			continue;
		}
		if(t == 2)
		{
			printf("1\n");
			continue;
		}
		if(t < 10)
		{
			printf("%d\n",(t - 2) * 8);
			continue;
		}
		if(t == 10)
		{
			printf("4\n");
			continue;
		}
		if(t == 11)
		{
			printf("%d\n",(10 - 2) * 8 - 2);
			continue;
		}
		if(t == 12)
		{
			printf("72");
			continue;
		}
		if(t % 10 == 2)
		{
			printf("%d\n",(t / 10 - 2) * 8);
			continue;
		}
		if(t == 20)
		{
			printf("1\n");
			continue;
		}
		if(t <= 31)
		{
			printf("%d\n",(t - 2) * 8 - 1);
			continue;
		}
		int day = -2;
		for(int i = 2021;i < t + 2000;i++)
			day += is_run(i) ? 366 : 365;
		printf("%d\n",day * 8 + 3);
	}
}

