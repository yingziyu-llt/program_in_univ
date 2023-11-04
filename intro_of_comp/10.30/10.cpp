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

int mat1[1000][1000],mat2[1000][1000];
int ans[1000][1000];

int main()
{
#ifdef llt
	freopen("10.in","r",stdin);
	freopen("10.out","w",stdout);
#endif
	int a,b,c;
	scanf("%d%d%d",&a,&b,&c);
	for(int i = 1;i <= a;i++)
		for(int j = 1;j <= b;j++)
			scanf("%d",&mat1[i][j]);
	
	for(int i = 1;i <= b;i++)
		for(int j = 1;j <= c;j++)
			scanf("%d",&mat2[i][j]);

	for(int i = 1;i <= a;i++)
		for(int j = 1;j <= c;j++)
			for(int k = 1;k <= b;k++)
				ans[i][j] += mat1[i][k] * mat2[k][j];
	for(int i = 1;i <= a;i++)
	{
		for(int j = 1;j < c;j++)
			printf("%d ",ans[i][j]);
		printf("%d\n",ans[i][c]);
	}
}

