/*
 * Copyright (C) 2023 Letian Lin <yingziyu-Lin@outlook.com>
 *
 * Distributed under terms of the GNU AGPLv3+ license.
 * 
 * Run for our better future
 */

#include <iterator>
#include<stdio.h>
#include<algorithm>
#include<string.h>
#include<math.h>
#include<queue>
using namespace std;

char map[100][100];

queue <pair<int,int> > q;
int dx[] = {0,0,1,0,-1};
int dy[] = {0,1,0,-1,0};

int main()
{
#ifdef llt
	freopen("6.in","r",stdin);
	freopen("6.out","w",stdout);
#endif
	int T;
	scanf("%d",&T);
	while(T--)
	{
		memset(map,'#',sizeof(map));
		int n;
		scanf("%d",&n);
		for(int i = 1;i <= n;i++)
			scanf("%s",map[i] + 1);
		int x = 0,y = 0;
		for(int i = 1;i <= n;i++)
		{
			bool flag = false;
			for(int j = 1;j <= n;j++)
				if(map[i][j] == 'e')
				{
					x = i;y = j;flag = true;
					break;
				}
			if(flag)
				break;
		}
		q.push(make_pair(x,y));
		
	}
}

