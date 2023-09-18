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

int main()
{
#ifdef llt
	freopen("LuoguP1077.in","r",stdin);
	freopen("LuoguP1077.out","w",stdout);
#endif
	int n,m;
	scanf("%d%d",&n,&m);
	dp[0] = 1;
	for(int i = 1;i <= n;i++)
	{
		int a;
		scanf("%d",&a);
		for(int j = m;j >= 0;j--)
			for(int k = 1;k <= a;k++)
				if(j - k >= 0)
					dp[j] = (dp[j] + dp[j - k]) % 1000007;
	}
	printf("%d",dp[m]);
	return 0;
}

