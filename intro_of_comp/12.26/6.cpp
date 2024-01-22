/*
 * Copyright (C) 2023 Letian Lin <yingziyu-Lin@outlook.com>
 *
 * Distributed under terms of the GNU AGPLv3+ license.
 * 
 * Run for our better future
 */

#include <functional>
#include<stdio.h>
#include<algorithm>
#include<string.h>
#include<math.h>
using namespace std;

int a[5100];
int dp[5100][5100];
int main()
{
#ifdef llt
	freopen("6.in","r",stdin);
	freopen("6.out","w",stdout);
#endif
	int n,k;
	scanf("%d%d",&n,&k);
	for(int i  = 1;i <= n;i++)
		scanf("%d",&a[i]);
	sort(a + 1, a + n + 1,greater<int>());
	for(int i = 1;i <= k;i++)
		dp[i * 3 - 1][i] = 1e6;
	dp[3][1] = (a[2] - a[3]) * (a[2] - a[3]);
	for(int i = 1;i <= n;i++)
		for(int j = 3 * i;j <= n;j++)
		{
			dp[j][i] = min(dp[j - 1][i],dp[j - 2][i - 1] + (a[j] - a[j - 1]) * (a[j] - a[j - 1]));
		}
	printf("%d",dp[n][k]);
}

