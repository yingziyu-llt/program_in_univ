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


int main()
{
#ifdef llt
	freopen("12.in","r",stdin);
	freopen("12.out","w",stdout);
#endif
	char s[10001];
	scanf("%s",s + 1);
	int len = strlen(s + 1);
	int cur = 0;
	int totn = 0,maxn = 0,minn = 998244353;
	for(int i = 1;i <= len;i++)
	{
		if(s[i] == ',')
			maxn = max(maxn,cur),minn = min(minn,cur),cur = 0,totn++;
		else
			cur = cur * 10 + s[i] - '0';
	}
	maxn = max(maxn,cur),minn = min(minn,cur),cur = 0,totn++;
	printf("%d\n%d",totn,maxn - minn);
}

