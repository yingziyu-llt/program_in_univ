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

int fa[100010];

int findfa(int x)
{
	return fa[x] = fa[x] == x ? x : findfa(fa[x]);
}

void merge(int a,int b)
{
	int faa = findfa(a),fab = findfa(b);
	fa[faa] = fab;
}

bool query(int a,int b)
{
	return findfa(a) == findfa(b);
}

int main()
{
#ifdef llt
	freopen("2.in","r",stdin);
	freopen("2.out","w",stdout);
#endif
	int n,m;
	scanf("%d %d",&n,&m);
	for(int i = 1;i <= n;i++)
		fa[i] = i;
	for(int i = 1;i <= m;i++)
	{
		char s[10];
		int a,b;
		scanf("%s %d%d",s,&a,&b);
		if(s[0] == 'M')
			merge(a,b);
		else
			printf("%s\n",query(a,b) ? "Yes" : "No");
	}
}

