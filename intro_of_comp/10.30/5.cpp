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

int a[100010];

int main()
{
#ifdef llt
	freopen("5.in","r",stdin);
	freopen("5.out","w",stdout);
#endif
	int n,m;
	scanf("%d%d",&n,&m);
	for(int i = 1;i <= n;i++)
		scanf("%d",&a[i]);
	for(int i = 1;i <= m;i++)
	{
		int last = a[n];
		for(int j = n;i >= 2;i--)
			a[j] = a[j - 1];
		a[1] = last;
	}
	for(int i = 1;i < n;i++)
		printf("%d ",a[i]);
	printf("%d",a[n]);
}

