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

int m,n,r,s;
int a[110][110],b[110][110];
int chk(int x,int y)
{
	int ans = 0;
	x--,y--;
	for(int i = 1;i <= r;i++)
		for(int j = 1;j <= s;j++)
			ans += abs(a[i + x][j + y] - b[i][j]);
	return ans;
}

int main()
{
#ifdef llt
	freopen("3.in","r",stdin);
	freopen("3.out","w",stdout);
#endif
	scanf("%d%d",&m,&n);
	for(int i = 1;i <= m;i++)
		for(int j = 1;j <= n;j++)
			scanf("%d",&a[i][j]);
	scanf("%d%d",&r,&s);
	for(int i = 1;i <= r;i++)
		for(int j = 1;j <= s;j++)
			scanf("%d",&b[i][j]);
	int minn = 100000000,x = 0,y = 0;
	for(int i = 1;i <= m - r + 1;i++)
		for(int j = 1;j <= n - s + 1;j++)
			if(chk(i,j) < minn)
				minn = chk(i,j),x = i,y = j;
	for(int i = 0;i < r;i++)
	{
		printf("%d",a[i + x][y]);
		for(int j = 1;j < s;j++)
			printf(" %d",a[i + x][j + y]);
		puts("");
	}
}
