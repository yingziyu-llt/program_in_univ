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

int mp[100][100];

int main()
{
#ifdef llt
	freopen("17.in","r",stdin);
	freopen("17.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	int curx = n / 2 + 1,cury = 1;
	mp[1][n / 2 + 1] = 1;
	for(int i = 2;i <= n * n;i++)
	{
		if(cury == 1)
		{
			if(curx == n)
				cury++;
			else
				cury = n,curx++;
		}
		else if(curx == n)
			cury--,curx = 1;
		else if(mp[cury - 1][curx + 1] != 0)
				cury++;
		else
			curx++,cury--;
		mp[cury][curx] = i;
	}
	for(int i = 1;i <= n;i++)
	{
		for(int j = 1;j < n;j++)
			printf("%d ",mp[i][j]);
		printf("%d",mp[i][n]);
		puts("");
	}
}

