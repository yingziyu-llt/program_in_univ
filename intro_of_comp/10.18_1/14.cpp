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

char s[1000][20];
int order[1000];
bool cmp(int a,int b)
{
	return strcmp(s[a],s[b]) < 0;
}
int res[1000];
int main()
{
#ifdef llt
	freopen("14.in","r",stdin);
	freopen("14.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	for(int i = 1;i <= n;i++)
	{
		char ss[20];
		scanf("%s",ss);
		for(int j = 0;j < (int)strlen(ss);j++)
			s[i][strlen(ss) - j - 1] = (ss[j] >= 'a' && ss[j] <= 'z') ? (ss[j] - 'a' + 3) % 26 + 'A' : (ss[j] - 'A' + 3) % 26 + 'a';
	}
	for(int i = 0;i <= n;i++)
		order[i] = i;
	sort(order + 1,order + n + 1,cmp);
	for(int i = 1;i <= n;i++)
		puts(s[order[i]]);
}

