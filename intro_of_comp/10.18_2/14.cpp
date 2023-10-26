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

bool mp[1100][1100];
int main()
{
#ifdef llt
	freopen("14.in","r",stdin);
	freopen("14.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	for(int i = 1;i <= n;i++)
	{
		for(int j = 1;j <= n;j++)
		{
			int k;
			scanf("%d",&k);
			mp[i][j] = k == 0;
		}
	}
	int fx,fy,ex,ey;
	for(int i = 1;i <= n;i++)
		for(int j = 1;j <= n;j++)
		{
			if(mp[i + 1][j] && mp[i][j + 1] && mp[i][j])
				fx = i,fy = j;
			if(!mp[i + 1][j] && !mp[i][j + 1] && mp[i][j])
				ex = i,ey = j;
		}
	printf("%d",(ex - fx - 1) * (ey - fy - 1));
}

