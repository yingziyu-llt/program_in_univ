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
	freopen("3.in","r",stdin);
	freopen("3.out","w",stdout);
#endif
	int k;
	scanf("%d",&k);
	for(int i = 1;i <= k;i++)
	{
		int m,n;
		scanf("%d%d",&n,&m);
		int a = 1;
		bool flag = 0;
		for(;a < m;a++)
		{
			if(n % a == 0 && n % (m - a) == 0)
			{
				flag = 1;
				break;
			}
		}
		if(flag)
			printf("%d",a);
		else
			printf("%d",-1);
		puts("");
	}
}

