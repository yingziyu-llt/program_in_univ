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
#include<vector>
using namespace std;

vector <int> a[110];

int main()
{
#ifdef llt
	freopen("18.in","r",stdin);
	freopen("18.out","w",stdout);
#endif
	int N;
	scanf("%d",&N);
	for(int i = 1;i <= N;i++)
	{
		int f,k;
		scanf("%d%d",&f,&k);
		for(int j = 1;j <= k;j++)
		{
			int t;
			scanf("%d",&t);
			a[t].push_back(f);
		}
	}
	int maxn = 0,mn = 0;
	for(int i = 1;i <= 100;i++)
	{
		if(maxn < (int)a[i].size())
			maxn = a[i].size(),mn = i;
	}
	printf("%d\n",mn);
	sort(a[mn].begin(),a[mn].end());
	printf("%d",a[mn][0]);
	for(int i = 1;i < a[mn].size();i++)
		if(a[mn][i - 1] != a[mn][i])
			printf(" %d",a[mn][i]);
}

