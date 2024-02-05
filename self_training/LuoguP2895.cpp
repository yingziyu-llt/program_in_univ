/*
 * Copyright (C) 2024 Letian Lin <yingziyu-Lin@outlook.com>
 *
 * Distributed under terms of the GNU AGPLv3+ license.
 * 
 * Run for our better future!
 */

#include <queue>
#include<stdio.h>
#include<algorithm>
#include<string.h>
#include<math.h>
#include <utility>
using namespace std;

int mp[1000][1000];

void upd(int x,int y,int t)
{
	mp[x][y] = min(mp[x][y],t);
	mp[x + 1][y] = min(mp[x + 1][y],t);
	if(x >= 0)
		mp[x - 1][y] = min(mp[x - 1][y],t);
	if(y >= 0)
		mp[x][y - 1] = min(mp[x][y - 1],t);
	mp[x][y + 1] = min(mp[x][y + 1],t);
}

queue <pair<pair<int,int>,int> > q; 

int main()
{
#ifdef llt
	freopen("LuoguP2895.in","r",stdin);
	freopen("LuoguP2895.out","w",stdout);
#endif
	int m;
	scanf("%d",&m);
	memset(mp,0x3f,sizeof(mp));
	for(int i = 1;i <= m;i++)
	{
		int x,y,t;
		scanf("%d%d%d",&x,&y,&t);
		upd(x,y,t);
	}
	q.push(make_pair(make_pair(0,0),0));
}

