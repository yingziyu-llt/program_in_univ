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

int a[100010],b[100010];
int main()
{
#ifdef llt
	freopen("4.in","r",stdin);
	freopen("4.out","w",stdout);
#endif
	int T;
	scanf("%d",&T);
	while(T--)
	{
		int k;
		scanf("%d",&k);
		char s1[110],s2[110];
		scanf("%s %s",s1,s2);
		memset(a,0,sizeof(a));
		memset(b,0,sizeof(b));
		int len1 = strlen(s1),len2 = strlen(s2);
		int len = max(len1,len2) - 3;
		for(int i = 2;i < len1;i++)
			a[i - 2] = s1[i] - '0';
		for(int i = 2;i < len2;i++)
			b[i - 2] = s2[i] - '0';
		for(int i = len;i > 0;i--)
		{
			a[i] += b[i];
			a[i - 1] += a[i] / k;
			a[i] %= k;
		}
		a[0] += b[0];
		printf("%d.",a[0] / k);
		a[0] %= k;
		while(len >= 0 && a[len] == 0)
			len--;
		for(int i = 0;i <= len;i++)
			printf("%d",a[i]);
		puts("");
	}
}

