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

int a[1010];

int main()
{
#ifdef llt
	freopen("6.in","r",stdin);
	freopen("6.out","w",stdout);
#endif
	int T;
	scanf("%d",&T);
	while(T--)
	{
		int n;
		scanf("%d",&n);
		for(int i = 1;i <= n;i++)
			scanf("%d",&a[i]);
		bool flag = true;
		if(a[1] != a[2]) printf("0"),flag = false;
		for(int i = 2;i < n;i++)
			if((a[i] - a[i - 1]) * (a[i + 1] - a[i]) < 0)
			{
				if(flag)
					printf("%d",i - 1),flag = false;
				else
					printf(" %d",i - 1);
			}
		if(a[n] != a[n - 1])
		{
			if(flag) printf("%d",n - 1);
			else     printf(" %d",n - 1);
		}
		puts("");
	}
}

