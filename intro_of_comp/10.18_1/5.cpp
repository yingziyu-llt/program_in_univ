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

int cnt[100];

int main()
{
#ifdef llt
	freopen("5.in","r",stdin);
	freopen("5.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	while(n--)
	{
		char s[1010];
		memset(s,0,sizeof(s));
		memset(cnt,0,sizeof(cnt));
		scanf("%s",s + 1);
		for(int i = 1;i <= strlen(s + 1);i++)
			cnt[s[i] - 'a']++;
		int maxn = 0,ans = 0;
		for(int i = 0;i <= 80;i++)
			if(cnt[i] > ans)
				maxn = i,ans = cnt[i];
		printf("%c %d\n",maxn + 'a',ans);
	}
}

