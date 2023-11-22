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
#include<queue>
using namespace std;

queue <pair<pair<int,int>,int> > q;

int map[100][100];
int step[100][100];
int dx[] = {0,1,1,-1,-1,2,2,-2,-2};
int dy[] = {0,2,-2,2,-2,1,-1,1,-1};

int main()
{
#ifdef llt
	freopen("8.in","r",stdin);
	freopen("8.out","w",stdout);
#endif
	char s1[10],s2[10];
	while(~scanf("%s %s",s1,s2))
	{
		while(!q.empty())
			q.pop();
		int x1 = s1[0] - 'a' + 1,x2 = s2[0] - 'a' + 1;
		int y1 = s1[1] - '0',y2 = s2[1] - '0';
		q.push(make_pair(make_pair(x1,y1),1));
		q.push(make_pair(make_pair(x2,y2),2));
		memset(map,0,sizeof(map));
		memset(step,0,sizeof(step));
		map[x1][y1] = 1,map[x2][y2] = 2;
		int ans = 0;
		if(x1 == x2 && y1 == y2)
		{
			printf("To get from %s to %s takes %d knight moves.\n",s1,s2,ans);
			continue;
		}
		while(!q.empty())
		{
			int curx = q.front().first.first;
			int cury = q.front().first.second;
			int type = q.front().second;
			for(int i = 1;i <= 8;i++)
			{
				int xx = curx + dx[i],yy = cury + dy[i];
				if(xx > 0 && xx <= 8 && yy > 0 && yy <= 8)
				{
					if(map[xx][yy] == 0)
					{
						step[xx][yy] = step[curx][cury] + 1;
						map[xx][yy] = type;
						q.push(make_pair(make_pair(xx,yy),type));
					}
					else if(map[xx][yy] != type)
					{
						ans = step[xx][yy] + step[curx][cury] + 1;
						break;
					}
				}
			}
			if(ans != 0)
				break;
			q.pop();
		}
		printf("To get from %s to %s takes %d knight moves.\n",s1,s2,ans);
	}
}

