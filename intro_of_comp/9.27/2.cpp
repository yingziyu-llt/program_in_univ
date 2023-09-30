/*
 * Copyright (C) 2023 Letian Lin <yingziyu-Lin@outlook.com>
 *
 * Distributed under terms of the GNU AGPLv3+ license.
 * 
 * Run for our better future
 */

#include <cstdio>
#include<stdio.h>
#include<algorithm>
#include<string.h>
#include<math.h>
using namespace std;


int main()
{
#ifdef llt
	freopen("2.in","r",stdin);
	freopen("2.out","w",stdout);
#endif
	int odd[310],even[310],cnt1 = 0,cnt2 = 0;
	int n;
	scanf("%d",&n);
	for(int i = 1;i <= n;i++)
	{
		int t;
		scanf("%d",&t);
		if(t&1)
			odd[++cnt1] = t;
		else
			even[++cnt2] = t;
		getchar();
	}
	printf("Odd=%d:",cnt1);
	for(int i = 1;i <= cnt1;i++)
	{
		printf("%d",odd[i]);
		putchar(i == cnt1 ? '\n' : ',');
	}
	if(cnt1 == 0) puts("");
	printf("Even=%d:",cnt2);
	for(int i = 1;i <= cnt2;i++)
	{
		printf("%d",even[i]);
		if(i != cnt2) putchar(',');
	}
}

