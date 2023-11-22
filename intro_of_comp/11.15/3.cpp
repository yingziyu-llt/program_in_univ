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

int dp[100010];

int f(int t)
{
	return dp[t] = dp[t] == 0 ? f(t - 2) + f(t - 1) : dp[t];
}

int main()
{
#ifdef llt
	freopen("3.in","r",stdin);
	freopen("3.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	dp[1] = 1,dp[0] = 1;
	printf("%d",f(n));
}

