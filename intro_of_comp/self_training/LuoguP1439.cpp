/*
 * Copyright (C) 2023 Letian Lin <yingziyu-Lin@outlook.com>
 *
 * Distributed under terms of the GNU AGPLv3+ license.
 * 
 * Run for our better future
 */

#include <cstring>
#include<stdio.h>
#include<algorithm>
#include<string.h>
#include<math.h>
using namespace std;
int b[100010];
int dp[100010];
int mp[100010];
int main()
{
#ifdef llt
	freopen("LuoguP1439.in","r",stdin);
	freopen("LuoguP1439.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	{
	int tmp;
	for(int i = 1;i <= n;i++)
		scanf("%d",&tmp),mp[tmp] = i;
	}
	for(int i = 1;i <= n;i++)
		scanf("%d",&b[i]);
	for(int i = 1;i <= n;i++)
		b[i] = mp[b[i]];
	memset(dp,0x3f,sizeof(dp));
	dp[1] = b[1];
	for(int i = 2;i <= n;i++)
	{
		int pos = upper_bound(dp + 1,dp + 1 + n, b[i]) - dp - 1;
		dp[pos + 1] = min(dp[pos + 1],b[i]);
	}
	printf("%d",(lower_bound(dp + 1,dp + n + 1,1000010)) - dp - 1);
}

