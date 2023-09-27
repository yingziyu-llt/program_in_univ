/*
 * Copyright (C) 2023 Letian Lin <yingziyu-Lin@outlook.com>
 *
 * Distributed under terms of the GNU AGPLv3+ license.
 * 
 * Run for our better future
 */

#include <deque>
#include<stdio.h>
#include<algorithm>
#include<string.h>
#include<math.h>
#include<queue>
#include <utility>
using namespace std;
deque <pair<int,int> > q;
int a[100010];
int dp[100010];
int main()
{
#ifdef llt
	freopen("LuoguP1735.in","r",stdin);
	freopen("LuoguP1735.out","w",stdout);
#endif
	int n,l,r;
	scanf("%d%d%d",&n,&l,&r);
	for(int i = 1;i <= n;i++)
		scanf("%d",&a[i]);
	dp[1] = a[1];
	q.push_back(make_pair(1,a[1]);
	for(int i = 1;i <= r - l;i++)
	{
		if(q.back().second < i)
			q.push_back(make_pair(i,a[i]));
	}

}

