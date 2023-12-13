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

char s[500];
int a[300],b[300];
int main()
{
#ifdef llt
	freopen("3.in","r",stdin);
	freopen("3.out","w",stdout);
#endif
	int T;
	scanf("%d",&T);
	while(T--)
	{
		memset(s,0,sizeof(s));
		memset(a,0,sizeof(a));
		memset(b,0,sizeof(b));
		scanf("%s",s);
		int len = strlen(s);
		int len1;
		for(int i = 0;i < len;i++)
			if(s[i] == '-')
			{
				len1 = i;
				break;
			}
			else
			{
				len1 = 0;
			}
		s[len1] = 0;
		bool smaller = 0;
		if(len1 < len - len1 - 1)
			smaller = 1;
		else if(len1 == len - len1 - 1 && strcmp(s,s + len1 + 1) < 0)
			smaller = 1;
		if(strcmp(s,s + len1 + 1) == 0)
		{
			printf("0\n");
			continue;
		}
		if(smaller)
		{
			printf("-");
			for(int i = 0;i < len1;i++)
				b[i] = s[i] - '0';
			for(int i = 0;i + len1 + 1 < len;i++)
				a[i] = s[i + len1 + 1] - '0';
		}
		else
		{
			for(int i = 0;i < len1;i++)
				a[i] = s[i] - '0';
			for(int i = 0;i + len1 + 1 < len;i++)
				b[i] = s[i + len1 + 1] - '0';
		}
		int len2 = min(len - len1 - 1,len1);
		len1 = max(len1,len - len1 - 1);
		for(int i = len2,j = len1;i >= 0 && j >= 0;i--,j--)
		{
			if(a[j] < b[i])
				a[j - 1]--,a[j] += 10;
			a[j] -= b[i];
		}
		int i = len1 - 1;
		while(i > 0)
		{
			if(a[i] < 0)
				a[i - 1]--,a[i] += 10;
			i--;
		}
		i = 0;
		while(a[i] == 0) i++;
		for(;i < len1;i++)
			printf("%d",a[i]);
		puts("");
	}
}

