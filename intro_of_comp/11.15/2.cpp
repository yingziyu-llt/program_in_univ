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
#include <utility>
using namespace std;

queue <pair<int,int> > q;

int map[100][100];
int dx[4] = {0,1,0,-1},dy[4] = {1,0,-1,0};

int main()
{
#ifdef llt
	freopen("2.in","r",stdin);
	freopen("2.out","w",stdout);
#endif
	int m,n;
	scanf("%d%d",&m,&n);
	pair<int,int> pos = make_pair(0,0);
	for(int i = 1;i <= m;i++)
		for(int j = 1;j <= n;j++)
			scanf("%d",&map[i][j]),pos = map[i][j] == 2 ? make_pair(i,j) : pos;
	q.push(pos);
	bool flag = 0;
	while(!q.empty())
	{
		int x = q.front().first,y = q.front().second;
		q.pop();
		for(int i = 0;i < 4;i++)
			if(x + dx[i] > 0 && x + dx[i] <= m && y + dy[i] > 0 && y + dy[i] <= n)
			{
				int xx = x + dx[i],yy = y + dy[i];
				if(map[xx][yy] == 0)
					q.push(make_pair(x + dx[i],y + dy[i])),map[xx][yy] = -1;
				else if(map[xx][yy] == 3)
					flag = 1;
			}
		if(flag) break;
	}
	if(flag)
		printf("Yes");
	else
		printf("No");

}

