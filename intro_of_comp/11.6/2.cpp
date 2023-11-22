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

int map[600][600];

int main()
{
#ifdef llt
	freopen("2.in","r",stdin);
	freopen("2.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	for(int _i = 1;_i <= n;_i++)
	{
		int op,x1,x2,y1,y2;
		scanf("%d%d%d%d%d",&op,&x1,&y1,&x2,&y2);
		if(op == 0)
			for(int i = y1;i <= y2;i++)
				for(int j = x1;j <= x2;j++)
					map[i][j] = 1;
		else
		{
			bool flag = 1;
			for(int i = y1;i <= y2;i++)
			{
				for(int j = x1;j <= x2;j++)
					if(map[i][j] == 0)
					{
						flag = false;
						break;
					}
				if(flag == false)
					break;
			}
			if(flag) puts("No");
			else     puts("Yes");
		}
	}
}

