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

char map[30][30];
bool vis[30][30];
int key[10];
int dx[] = {0,0,1,0,-1};
int dy[] = {0,1,0,-1,0};

int main()
{
#ifdef llt
	freopen("OpenJudge1159.in","r",stdin);
	//freopen("OpenJudge1159.out","w",stdout);
#endif
	while(1)
	{
		int n,m;
		scanf("%d%d",&n,&m);
		if(m == 0 && n == 0)
			break;
		memset(map,0,sizeof(map));
		for(int i = 1;i <= n;i++)
			scanf("%s",map[i] + 1);
		int x = 0,y = 0;
		memset(key,0,sizeof(key));
		for(int i = 1;i <= n;i++)
			for(int j = 1;j <= m;j++)
			{
				if(map[i][j] == 'S')
					x = i,y = j;
				if(map[i][j] >= 'a' && map[i][j] <= 'e')
					key[map[i][j] - 'a']--;
			}
		map[x][y] = '.';
		bool success = false;
		while(1)
		{
			bool flag = false;
			queue <pair<int,int> > q;
			while(!q.empty()) q.pop();
			q.push(make_pair(x,y));
			memset(vis,0,sizeof(vis));
			vis[x][y] = 1;
			while(!q.empty())
			{
				int nowx = q.front().first,nowy = q.front().second;
				q.pop();
				for(int i = 1;i <= 4;i++)
				{
					int xx = nowx + dx[i],yy = nowy + dy[i];
					if(map[xx][yy] >= 'a' && map[xx][yy] <= 'e')
					{
						key[map[xx][yy] - 'a']++;
						if(key[map[xx][yy] - 'a'] == 0) flag = 1;
						map[xx][yy] = '.';
					}
					if(map[xx][yy] >= 'A' && map[xx][yy] <= 'E')
						if(key[map[xx][yy] - 'A'] == 0)
							map[xx][yy] = '.',flag = true;
					if(map[xx][yy] == 'G')
					{
						success = 1;
						break;
					}
					if(map[xx][yy] == '.' && !vis[xx][yy])
						q.push(make_pair(xx,yy)),vis[xx][yy] = 1;
				}
				if(success) break;
			}
			while(!q.empty()) q.pop();
			if(success) break;
			if(!flag) break;
		}
		if(success) printf("YES\n");
		else        printf("NO\n");
	}
}

