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
#include<queue>
#include <utility>
using namespace std;

char mp[110][110];

int main()
{
#ifdef llt
	freopen("1.in","r",stdin);
	freopen("1.out","w",stdout);
#endif
	int n,m;
	scanf("%d%d",&m,&n);
	for(int i = 1;i <= n;i++)
		scanf("%s",mp[i] + 1);
	pair<int,int> s,t;
	for(int i = 1;i <= n;i++)
		for(int j = 1;j <= m;j++)
		{
			if(mp[i][j] == 'Z')
				s.first = i,s.second = j,mp[i][j] = '#';
			if(mp[i][j] == 'T')
				t.first = i,t.second = j,mp[i][j] = '#';
		}
	queue <pair<int,int> > q;
	q.push(s);
	bool suc = 0;
	while(!q.empty())
	{
		int x = q.front().first,y = q.front().second;
		q.pop();
		int dx[] = {0,0,1,0,-1},dy[] = {0,1,0,-1,0};
		if(x == t.first && y == t.second)
		{
			suc = 1;
			break;
		}
		for(int i = 1;i <= 4;i++)
		{
			int xx = x + dx[i],yy = y + dy[i];
			if(mp[xx][yy] == '#')
				q.push(make_pair(xx,yy)),mp[xx][yy] = ' ';
		}
	}
	if(suc)
		puts("yEs");
	else
		puts("NnO");
}

