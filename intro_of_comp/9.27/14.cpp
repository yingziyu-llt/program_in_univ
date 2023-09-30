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
	freopen("14.in","r",stdin);
	freopen("14.out","w",stdout);
#endif
	int m;
	int l,h;
	int cnt = 0;
	scanf("%d",&m);
	scanf("%d%d",&l,&h);
	for(int i = 1;i <= m;i++)
	{
		int t;
		scanf("%d",&t);
		if(l <= t && t <= h)
			cnt++;
	}
	printf("%d",cnt);

}

