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


int main()
{
#ifdef llt
	freopen("18.in","r",stdin);
	freopen("18.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	for(int i = 1;i <= n;i += 2)
	{
		printf("$");
		for(int j = 1;j <= n / 2  - i / 2;j++)
			printf(" ");
		for(int j = 1;j <= i;j++)
			printf("*");
		puts("");
	}
	for(int i = n - 2;i >= 1;i -= 2)
	{
		printf("$");
		for(int j = 1;j <= n / 2  - i / 2;j++)
			printf(" ");
		for(int j = 1;j <= i;j++)
			printf("*");
		puts("");
	}
}

