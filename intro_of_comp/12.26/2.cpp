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

bool a[100100],b[100100];

int main()
{
#ifdef llt
	freopen("2.in","r",stdin);
	freopen("2.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	for(int i = 1;i <= n;i++)
	{
		int x,y;
		scanf("%d%d",&x,&y);
		a[x] = 1,b[y] = 1;
	}
	bool flag = 1;
	for(int i = 1;i <= 100000;i++)
		if(a[i] && !b[i])
		{
			if(flag)
				printf("%d",i),flag = 0;
			else
				printf(", %d",i);
		}
}

