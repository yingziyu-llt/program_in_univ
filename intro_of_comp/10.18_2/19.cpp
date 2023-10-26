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

int maxy[110];
int ansx[110],anscnt;
int main()
{
#ifdef llt
	freopen("19.in","r",stdin);
	freopen("19.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	for(int i = 1;i <= n;i++)
	{
		int x,y;
		scanf("%d%d",&x,&y);
		maxy[x] = max(maxy[x],y);
	}
	int maxn = 0;
	for(int i = 100;i >= 0;i--)
		if(maxn < maxy[i])
			++anscnt,ansx[anscnt] = i,maxn = maxy[i];
	for(int i = anscnt;i > 1;i--)
		printf("(%d,%d),",ansx[i],maxy[ansx[i]]);
	printf("(%d,%d)",ansx[1],maxy[ansx[1]]);
}

