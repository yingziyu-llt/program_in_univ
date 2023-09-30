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
	int n;
	scanf("%d",&n);
	int ans = 0;
	for(int i = 1;i <= n;i++)
	{
		int t;
		scanf("%d",&t);
		ans = ans + (int)((t % 10 - (t / 10 % 10 + t / 100 % 10 + t / 1000 % 10)) > 0);
	}
	printf("%d",ans);
}

