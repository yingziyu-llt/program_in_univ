/*
 * Copyright (C) 2023 Letian Lin <yingziyu-Lin@outlook.com>
 *
 * Distributed under terms of the GNU AGPLv3+ license.
 * 
 * Run for our better future
 */

#include<stdio.h>
#include<queue>
#include<algorithm>
#include<string.h>
#include<math.h>
#include <tuple>
#include <utility>
using namespace std;

int n,m;
char map[100][100];
int dx[] = {0,0,1,0,-1};
int dy[] = {0,1,0,-1,0};

int main()
{
#ifdef llt
	freopen("20.in","r",stdin);
	freopen("20.out","w",stdout);
#endif
	int cnt1 = 0;
	while(1)
	{
		scanf("%d%d",&n,&m);//m行n列
		if(m == 0 && n == 0) break;
		printf("Board #%d:\n",++cnt1);
		memset(map,0,sizeof(map));
		for(int i = 1;i <= m;i++)
		{
			getchar();
			for(int j = 1;j <= n;j++)
				map[i][j] = getchar();
		}
		int xxcnt = 0;
		while(1)
		{
			int x1,y1,x2,y2;
			scanf("%d%d%d%d",&x1,&y1,&x2,&y2);
			swap(x1,y1),swap(x2,y2);
			if(x1 == 0 && x2 == 0 && y1 == 0 && y2 == 0)	
				break;
			xxcnt++;
			int dist = 0x7fffffff;
			{
				int dis[100][100];
				memset(dis,-1,sizeof(dis));
				priority_queue <pair<pair<int,int>,pair<int,int> > > q;
				while(!q.empty()) q.pop();
				q.push(make_pair(make_pair(0,0),make_pair(x1,y1)));
				dis[x1][y1] = 0;
				while(!q.empty())
				{
					int cnt = q.top().first.first,dir = q.top().first.second;
					int x = q.top().second.first,y = q.top().second.second;
					dis[x][y] = -cnt;
					q.pop();
					for(int i = 1;i <= 4;i++)
					{
						int vx = x + dx[i],vy = y + dy[i];
						if(vx >= 0 && vx <= m + 1 && vy >= 0 && vy <= n + 1)
						{
							if(map[vx][vy] != 'X' && dis[vx][vy] == -1)
							{
								q.push(make_pair(make_pair(dir == i ? cnt : cnt - 1,i),make_pair(vx,vy)));
								dis[vx][vy] = 0;
							}
							if(vx == x2 && vy == y2)
								dist = min(dist,-cnt + (dir == i ? 0 : 1));
						}
					}
				}
			}

			if(dist == 0x7fffffff)
				printf("Pair %d: impossible.",xxcnt);
			else
				printf("Pair %d: %d segments.",xxcnt,dist);
			puts("");
		}
		puts("");
	}
}

