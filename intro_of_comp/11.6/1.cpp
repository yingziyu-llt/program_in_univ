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

int tag[10000];

int main()
{
#ifdef llt
	freopen("1.in","r",stdin);
	freopen("1.out","w",stdout);
#endif
	int l,m;
	scanf("%d%d",&l,&m);
	for(int i = 1;i <= m;i++)
	{
		int a,b;
		scanf("%d%d",&a,&b);
		tag[a] += 1,tag[b + 1] -= 1;
	}
	int ans = 0;
	int now = 0;
	for(int i = 0;i <= l;i++)
	{
		now += tag[i];
		if(now == 0) ans++;
	}
	printf("%d",ans);
}

