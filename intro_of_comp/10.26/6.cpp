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
int sum[100010];
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
			sum[i] = 0;
		for(int i = 1;i <= n;i++)
			scanf("%d",&a[i]),sum[i] = sum[i - 1] + a[i];
		int ans = 0x7fffffff;
		for(int i = 1;i < n;i++)
			ans = min(ans,((i - 1) * a[i] - sum[i - 1] + sum[n] - sum[i] - (n - i) * a[i]));
		printf("%d\n",ans);
	}
}

