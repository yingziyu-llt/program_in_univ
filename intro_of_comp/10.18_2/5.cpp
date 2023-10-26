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

bool lantern[1010];

int main()
{
#ifdef llt
	freopen("5.in","r",stdin);
	freopen("5.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	for(int i = 2;i <= n;i++)
		for(int j = 1;j * i <= n;j++)
			lantern[i * j] = !lantern[i * j];
	for(int i = 1;i <= n;i++)
		if(lantern[i])
			printf("%d ",i);
}

