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

bool used[100];
char s[100010];
queue <char> q;
int main()
{
#ifdef llt
	freopen("4.in","r",stdin);
	freopen("4.out","w",stdout);
#endif
	int t;
	scanf("%d",&t);
	while(t--)
	{
		memset(used,0,sizeof(used));
		memset(s,0,sizeof(s));
		scanf("%s",s + 1);
		for(int i = 1;i <= strlen(s + 1);i++)
		{
			if(s[i] == q.front())
				q.pop();
			if(used[s[i] - 'a'] == 0)
			{
				q.push(s[i]);
				used[s[i] - 'a'] = 1;
			}
		}
		if(q.empty())
			printf("no");
		else
			putchar(q.front());
		puts("");
		while(!q.empty())
			q.pop();
	}
}

