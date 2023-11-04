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

int m[2][13] = {{0,31,28,31,30,31,30,31,31,30,31,30,31},{0,31,29,31,30,31,30,31,31,30,31,30,31}};
int is_run(int a)
{
	if(a % 4 != 0)
		return 0;
	if(a % 400 == 0)
		return 1;
	if(a % 100)
		return 1;
	return 0;
}

int main()
{
#ifdef llt
	freopen("12.in","r",stdin);
	freopen("12.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	while(n--)
	{
		int y,m1,m2;
		scanf("%d%d%d",&y,&m1,&m2);
		if(m1 > m2) swap(m1,m2);
		int day = 0;
		for(int i = m1;i < m2;i++)
			day += m[is_run(y)][i];
		printf("%s\n",day % 7 ? "NO" : "YES");
	}
}

