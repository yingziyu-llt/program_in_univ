/*
 * Copyright (C) 2023 Letian Lin <yingziyu-Lin@outlook.com>
 *
 * Distributed under terms of the GNU AGPLv3+ license.
 * 
 * Run for our better future
 */

#include <functional>
#include<stdio.h>
#include<algorithm>
#include<string.h>
#include<math.h>
#include<queue>
#include <vector>
using namespace std;

priority_queue <int,vector<int>,greater<int> > q;

int main()
{
#ifdef llt
	freopen("9.in","r",stdin);
	freopen("9.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	for(int i = 1;i <= n;i++)
	{
		int t;
		scanf("%d",&t);
		q.push(t);
	}
	int m;
	scanf("%d",&m);
	for(int i = 1;i <= m;i++)
	{
		char s[100];int t;
		scanf("%s",s);
		if(s[0] == 'E')
		{
			if(q.empty())
			{
				printf("NULL\n");
				continue;
			}
			printf("%d\n",q.top());
			q.pop();
		}
		else
		{
			scanf("%d",&t);
			q.push(t);
		}
	}
}

