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

pair <int,int> st[160000];

int main()
{
#ifdef llt
	freopen("LuoguP1868.in","r",stdin);
	freopen("LuoguP1868.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	for(int i = 1;i <= n;i++)
		scanf("%d%d",&st[i].second,&st[i].first);
	sort(st + 1,st + 1 + n);
	int iter = 0;
	for(int i = 1;i <= n;i++)
	{
		for(int j = st[i].second;j >= st[i] 
	}
}

