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
	freopen("1.in","r",stdin);
	freopen("1.out","w",stdout);
#endif
	int t;
	scanf("%d",&t);
	while(t != 1)
	{
		if(t & 1)
			printf("%d*3+1=%d\n",t,t * 3 + 1),t = t * 3 + 1;
		else
			printf("%d/2=%d",t,t / 2),t /= 2;
	}
	puts("End");
}

