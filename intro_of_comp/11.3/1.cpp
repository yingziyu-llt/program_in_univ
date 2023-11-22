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

int a[100010];
int b[100010];

int n,m;

void input()
{
	scanf("%d%d",&n,&m);
	for(int i = 1;i <= n;i++)
		scanf("%d",a + i);
	for(int i = 1;i <= m;i++)
		scanf("%d",b + i);
}

void work()
{
	sort(a + 1,a + 1 + n);
	sort(b + 1,b + 1 + m);
}

void out()
{
	for(int i = 1; i <= n;i++)
		printf("%d ",a[i]);
	for(int i = 1; i < m;i++)
		printf("%d ",b[i]);
	printf("%d",b[m]);
}

int main()
{
#ifdef llt
	freopen("1.in","r",stdin);
	freopen("1.out","w",stdout);
#endif
	input();
	work();
	out();
}

