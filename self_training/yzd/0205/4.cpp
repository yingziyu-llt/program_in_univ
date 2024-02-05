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
using namespace std;

char mp[100][100];

queue <pair<pair<int,int>,int> > q;
int main()
{
#ifdef llt
	freopen("4.in","r",stdin);
	freopen("4.out","w",stdout);
#endif
	while(1)
	{
		int n,m;
		scanf("%d%d",&n,&m);
		if(m == 0 && n == 0)break;
		memset(mp,0,sizeof(mp));
		for(int i = 1;i <= n;i++)
			scanf("%s",mp[i] + 1);
		pair<int,int> s,t;
		for(int i = 1;i <= n;i++)
			for(int j = 1;j <= m;j++)
			{
				if(mp[i][j] == '@')
					s.first = i,s.second = j,mp[i][j] = '.';
				if(mp[i][j] == '*')
					t.first = i,t.second = j,mp[i][j] = '.';
			}
		while(!q.empty()) q.pop();
		q.push(make_pair(s,0));
		int suc = 0;
		while(!q.empty())
		{
			int x = q.front().first.first,y = q.front().first.second,val = q.front().second;
			q.pop();
			int dx[] = {0,0,1,0,-1},dy[] = {0,1,0,-1,0};
			if(x == t.first && y == t.second)
			{
				suc = val;
				break;
			}
			for(int i = 1;i <= 4;i++)
			{
				int xx = x + dx[i],yy = y + dy[i];
				if(mp[xx][yy] == '.')
					q.push(make_pair(make_pair(xx,yy),val + 1)),mp[xx][yy] = ' ';
			}
		}
		if(suc)
			printf("%d\n",suc);
		else
			puts("-1");

	}
}
