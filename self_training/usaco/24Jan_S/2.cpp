/*
 * Copyright (C) 2024 Letian Lin <yingziyu-Lin@outlook.com>
 *
 * Distributed under terms of the GNU AGPLv3+ license.
 * 
 * Run for our better future!
 */

#include<stdio.h>
#include<algorithm>
#include<string.h>
#include<math.h>
using namespace std;

struct Edge
{
	int to,nxt;
}e[100010];
int ecnt,ehead[100010];
void addedge(int a,int b)
{
	e[++ecnt] = (Edge){b,ehead[a]};
	ehead[a] = ecnt;
}
int input[100010],out[100010],fa[100010],soncnt[100010];
int dep[100010],maxdep;
void dep_upd(int x,int d)
{
	soncnt[x]++;
	if(fa[x] == 0)
	{
		dep[x] = max(dep[x],d);
		maxdep = max(maxdep,dep[x]);
	}
	if(dep[x] < d)
		dep[x] = d,dep_upd(fa[x],d + 1);
}
void upd(int x)
{
	if(out[x] == 0) return ;
	soncnt[x] = 0;
	for(int i = ehead[x];i;i = e[i].nxt)
		soncnt[x] += soncnt[e[i].to];
}
pair<int,int> cur[100010];
int main()
{
#ifdef llt
	freopen("2.in","r",stdin);
	freopen("2.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	for(int i = 1;i <= n;i++)
		scanf("%d",&input[i]);
	for(int i = 1;i < n;i++)
	{
		int x,y;
		scanf("%d%d",&x,&y);
		addedge(x,y);
		out[x]++,fa[y] = x;
	}
	int ans = 0;
	int m = 0;
	for(int i = 1;i <= n;i++)
		if(out[i] == 0)
			m++,dep_upd(i,1);
	for(int i = 1;i <= m;i++)
		cur[i] = make_pair(dep[input[i]],input[i]);
	sort(cur + 1,cur + 1 + m);
	for(int i = 1;i <= m;i++)
	{
		upd(cur[i].second);
		if(soncnt[cur[i].second] > 0)
			soncnt[cur[i].second]--,ans++;
	}
	printf("%d",ans);
}

