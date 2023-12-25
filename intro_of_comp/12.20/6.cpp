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
#include <utility>
#include <vector>
using namespace std;

int dp[10000],cpdp[10000];
vector<pair<int,int> > a[10000];
int main()
{
#ifdef llt
	freopen("6.in","r",stdin);
	freopen("6.out","w",stdout);
#endif
	int n,m;
	scanf("%d%d",&m,&n);
	for(int i = 1;i <= n;i++)
	{
		int v,w,x;
		scanf("%d%d%d",&w,&v,&x);
		a[x].push_back(make_pair(v,w));
	}
	for(int i = 1;i <= 9999;i++)
	{
		for(int j = 0;j <= m;j++)
			cpdp[j] = dp[j];
		for(auto& x : a[i])
			for(int k = m;k >= x.second;k--)
				cpdp[k] = max(cpdp[k],dp[k - x.second] + x.first);
		for(int j = 0;j <= m;j++)
			dp[j] = cpdp[j];
	}
	printf("%d",dp[m]);
}

