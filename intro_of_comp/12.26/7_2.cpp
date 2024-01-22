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

struct aa
{
	int from,to;
	bool operator <(aa x)
	{
		if(from == x.from)
			return to > x.to;
		return from < x.from;
	}
}a[510000];
int acnt = 0;
void addPoint(int pos,int l)
{
	a[++acnt] = {max(pos - l,1),pos + l};
}
int main()
{
#ifdef llt
	freopen("7_2.in","r",stdin);
	freopen("7_2.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	for(int i = 1;i <= n;i++)
	{
		int t;
		scanf("%d",&t);
		addPoint(i,t);
	}
	sort(a + 1,a + acnt + 1);
	int cur = 1,maxn = 1,ans = 0,i = 1;
	while(maxn < n)
	{
		maxn = cur;
		while(i <= n && a[i].from <= maxn + 1)
		{
			cur = max(cur,a[i].to);
			i++;
		}
		ans++;
		if(cur >= n)
			break;
	}
	printf("%d",ans);
	return 0;
}

