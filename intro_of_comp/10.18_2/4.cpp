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

int a[20010],cnt[200];

int main()
{
#ifdef llt
	freopen("4.in","r",stdin);
	freopen("4.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	for(int i = 1;i <= n;i++)
		scanf("%d",&a[i]),cnt[a[i]]++;
	if(cnt[a[1]] == 1)
		printf("%d",a[1]);
	else if(cnt[a[1]] != -1)
		printf("%d",a[1]),cnt[a[1]] = -1;
	for(int i = 2;i <= n;i++)
	{
		if(cnt[a[i]] == 1)
			printf(" %d",a[i]);
		else if(cnt[a[i]] != -1)
			printf(" %d",a[i]),cnt[a[i]] = -1;
	}
}

