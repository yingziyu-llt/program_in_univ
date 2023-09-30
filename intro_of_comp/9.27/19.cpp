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

int tot[100010];
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
		int t;
		scanf("%d",&t);
		tot[t]++;
	}
	int m;
	scanf("%d",&m);
	printf("%d",tot[m]);
}

