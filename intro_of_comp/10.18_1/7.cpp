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
#include<queue>
using namespace std;
queue <int> q[26];
int cnt[30];
int main()
{
#ifdef llt
	freopen("7.in","r",stdin);
	freopen("7.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	for(int i = 1;i <= n;i++)
	{
		int xx;
		char s[100];
		scanf("%d %s",&xx,s + 1);
		for(int j = 1;j <= strlen(s + 1);j++)
			cnt[s[j] - 'A']++,q[s[j] - 'A'].push(xx);
	}
	int maxn = 0,mx = 0;
	for(int i = 0;i <= 25;i++)
		if(mx < cnt[i])
			mx = cnt[i],maxn = i;
	printf("%c\n%d\n",'A' + maxn,mx);
	while(!q[maxn].empty())
		printf("%d\n",q[maxn].front()),q[maxn].pop();
}

