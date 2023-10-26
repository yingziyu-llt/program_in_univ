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

int tot[3000];
int main()
{
#ifdef llt
	freopen("11.in","r",stdin);
	freopen("11.out","w",stdout);
#endif
	char s[5000];
	scanf("%s",s + 1);
	int len = strlen(s + 1);
	int cur = 0;
	int totn = 1;
	for(int i = 1;i <= len;i++)
	{
		if(s[i] == ',')
			tot[cur]++,cur = 0,totn++;
		else
			cur = cur * 10 + s[i] - '0';
	}
	tot[cur]++;
	memset(s,0,sizeof(s));
	scanf("%s",s + 1);
	len = strlen(s + 1);
	cur = 0;
	for(int i = 1;i <= len;i++)
	{
		if(s[i] == ',')
			tot[cur]--,cur = 0;
		else
			cur = cur * 10 + s[i] - '0';
	}
	tot[cur]--;
	int maxn = tot[0];
	for(int i = 1;i <= 1100;i++)
		tot[i] += tot[i - 1],maxn = max(maxn,tot[i]);
	printf("%d %d",totn,maxn);
}
