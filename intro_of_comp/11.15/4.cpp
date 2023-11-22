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

int c[1000][1000];

int solve(int n,int m)
{
	if(n < m)
		return 0;
	if(n == 0 || n == m || m == 1)
		return 1;
	if(c[m][n] != 0)
		return c[m][n];
	return c[m][n] = solve(n - 1,m - 1) + m * solve(n - 1,m);
}

int main()
{
#ifdef llt
	freopen("4.in","r",stdin);
	freopen("4.out","w",stdout);
#endif
	int n;
	while(1)
	{
		scanf("%d",&n);
		if(n == -1)
			break;
		int ans = 0;
		for(int i = 1;i <= n;i++)
			ans += solve(n,i);
		if(n == 0) printf("1\n");
		else
		printf("%d\n",ans);
	}
}

