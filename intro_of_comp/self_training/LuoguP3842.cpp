/*
 * =template=.cpp
 * Copyright (C) 2023 Letian Lin <yingziyu-Lin@outlook.com>
 *
 * Distributed under terms of the GNU AGPLv3+ license.
 */

#include<stdio.h>
#include<algorithm>
#include<string.h>
#include<math.h>
using namespace std;
int dp[20010][2];
int l[20010],r[20010];
int main()
{
#ifdef llt
	freopen("LuoguP3842.in","r",stdin);
	freopen("LuoguP3842.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	for(int i = 1;i <= n;i++)
		scanf("%d%d",&l[i],&r[i]);
	l[0] = r[0] = 1;
	l[n + 1] = r[n + 1] = n;
	for(int i = 1;i <= n + 1;i++)
	{
		dp[i][0] = min(dp[i - 1][0] + (int)abs(l[i - 1] - r[i]) + (r[i] - l[i]), dp[i - 1][1] + (int)abs(r[i - 1] - r[i]) + (r[i] - l[i]));
		dp[i][1] = min(dp[i - 1][0] + (int)abs(l[i - 1] - l[i]) + (r[i] - l[i]), dp[i - 1][1] + (int)abs(r[i - 1] - l[i]) + (r[i] - l[i]));
	}
	printf("%d",dp[n + 1][0] + n - 1);
}

