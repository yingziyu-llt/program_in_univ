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
using namespace std;

int maxx[6],miny[6];
int a[6][6];
int main()
{
#ifdef llt
	freopen("21.in","r",stdin);
	freopen("21.out","w",stdout);
#endif
	memset(miny,0x3f,sizeof(miny));
	for(int i = 1;i <= 5;i++)
		for(int j = 1;j <= 5;j++)
			scanf("%d",&a[i][j]),maxx[i] = max(a[i][j],maxx[i]),miny[j] = min(miny[j],a[i][j]);
	for(int i = 1;i <= 5;i++)
		for(int j = 1;j <= 5;j++)
			if(maxx[i] == a[i][j] && miny[j] == a[i][j])
			{
				printf("%d %d %d",i,j,a[i][j]);
				return 0;
			}
	printf("not found");
}

