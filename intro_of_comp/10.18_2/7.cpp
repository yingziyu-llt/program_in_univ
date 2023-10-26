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
	freopen("7.in","r",stdin);
	freopen("7.out","w",stdout);
#endif
	while(1)
	{
		int n;
		scanf("%d",&n);
		if(n == 0) break;
		int pre,cnt = 1,maxn = 1,cur;
		scanf("%d",&pre);
		for(int i = 2;i <= n;i++)
		{
			scanf("%d",&cur);
			if(cur == pre) cnt++;
			else cnt = 1,pre = cur;
			maxn = max(maxn,cnt);
		}
		printf("%d\n",maxn);
	}
}

