/*
 * Copyright (C) 2024 Letian Lin <yingziyu-Lin@outlook.com>
 *
 * Distributed under terms of the GNU AGPLv3+ license.
 * 
 * Run for our better future!
 */

#include<stdio.h>
#include<algorithm>
#include<string.h>
#include<math.h>
using namespace std;

int a[100100];
bool type[1000100];

int main()
{
#ifdef llt
	freopen("1.in","r",stdin);
	freopen("1.out","w",stdout);
#endif
	int T;
	scanf("%d",&T);
	while(T--)
	{
		int n;
		scanf("%d",&n);
		for(int i = 1;i <= n;i++)
			scanf("%d",&a[i]);
		memset(type,0,sizeof(type));
		for(int i = 1;i < n;i++)
			if(a[i] == a[i + 1])
				type[a[i]] = 1;
		for(int i = 1;i < n - 1;i++)
			if(a[i] == a[i + 2])
				type[a[i]] = 1;
		bool success = 0;
		for(int i = 1;i <= n;i++)
			if(type[i])
			{
				if(!success)
					printf("%d",i),success = 1;
				else
					printf(" %d",i);
			}
		if(!success)
			printf("-1");
		puts("");
	}
}

