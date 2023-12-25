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

char input[20][20];
int n;

void chpos(int x,int y)
{
	if(x <= 0 || x > n || y < 0 || y > n) return ;
	if(input[x][y] == 'y') input[x][y] = 'w';
	else                   input[x][y] = 'y';
}
void change(int x,int y)
{
	chpos(x,y);
	chpos(x + 1,y);chpos(x,y + 1);
	chpos(x - 1,y);chpos(x,y - 1);
}
char cp_input[50][50];
int main()
{
#ifdef llt
	freopen("9.in","r",stdin);
	freopen("9.out","w",stdout);
#endif
	int T;
	scanf("%d",&T);
	while(T--)
	{
		scanf("%d\n",&n);
		int total = 1 << n;
		int ans = 0x7fffffff;
		for(int i = 1;i <= n;i++)
			scanf("%s",input[i] + 1);
		for(int i = 1;i <= n;i++)
			for(int j = 1;j <= n;j++)
				cp_input[i][j] = input[i][j];
		for(int k = 0;k < total;k++)
		{
			for(int i = 1;i <= n;i++)
				input[0][i] = (k & (1 << (i - 1))) != 0 ? 'y' : 'w';
			int tot = 0;
			for(int i = 1;i <= n;i++)
				for(int j = 1;j <= n;j++)
					if(input[i - 1][j] == 'w') change(i,j),tot++;
			bool flag = true;
			for(int i = 1;i <= n;i++)
				for(int j = 1;j <= n;j++)
					if(input[i][j] == 'w')
					{
						flag = false;
						break;
					}
			if(flag)
				ans = min(ans,tot);
			for(int i = 1;i <= n;i++)
				for(int j = 1;j <= n;j++)
					input[i][j] = cp_input[i][j];
		}
		if(ans > n * n)
			puts("inf");
		else
			printf("%d\n",ans);

	}
}

