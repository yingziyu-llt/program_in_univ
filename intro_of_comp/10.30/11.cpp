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

int month[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};

int main()
{
#ifdef llt
	freopen("11.in","r",stdin);
	freopen("11.out","w",stdout);
#endif
	int a;
	scanf("%d",&a);
	a--;
	a += 12;
	a %= 7;
	if(a == 4)
		printf("%d\n",1);
	for(int i = 1;i <= 11;i++)
	{
		a += month[i];
		a %= 7;
		if(a == 4)
			printf("%d\n",i + 1);

	}
}

