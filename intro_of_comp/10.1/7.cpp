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
	freopen("8.in","r",stdin);
	freopen("8.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	int ans = -1;
	for(int i = 1;i <= n;i++)
	{
		int t;
		scanf("%d",&t);
		ans = max(t,ans);
	}
	printf("%d",ans);
}

