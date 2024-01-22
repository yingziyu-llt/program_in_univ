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

int a[500010];
int dp[500010][2];

int main()
{
#ifdef llt
	freopen("7.in","r",stdin);
	freopen("7.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	for(int i = 1;i <= n;i++)
		scanf("%d",&a[i]);
	memset(dp,0x3f,sizeof(dp));
	dp[0][0] = dp[0][1] = 0;
	for(int i = 1;i <= n;i++)
	{
		dp[i][1] = min(dp[i][1],dp[max(0,i - a[i] - 1)][0] + 1);
		for(int j = max(i - a[i],1);j <= i + a[i] && j <= n;j++)
			dp[j][0] = min(dp[j][0],dp[i][1]);
	}
	printf("%d",min(dp[n][0],dp[n][1]));
}

