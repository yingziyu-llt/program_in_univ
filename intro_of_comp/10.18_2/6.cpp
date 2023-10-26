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

void chk(int a,int b,int c)
{
	bool used[10];
	int aa,bb,cc;
	aa = a,bb = b,cc = c;
	memset(used,0,sizeof(used));
	while(a)
	{
		if(used[a % 10]) return ;
		used[a % 10] = 1;
		if(a % 10 == 0) return ;
		a /= 10;
	}
	while(b)
	{
		if(used[b % 10]) return ;
		used[b % 10] = 1;
		if(b % 10 == 0) return ;
		b /= 10;
	}
	while(c)
	{
		if(used[c % 10]) return ;
		used[c % 10] = 1;
		if(c % 10 == 0) return ;
		c /= 10;
	}
	printf("%d %d %d\n",aa,bb,cc);
}

int main()
{
#ifdef llt
	freopen("6.in","r",stdin);
	freopen("6.out","w",stdout);
#endif
	for(int i = 1;i <= 9;i++)
		for(int j = 1;j <= 9;j++)
			for(int k = 1;k <= 9;k++)
			{
				int t = i * 100 + j *10 + k;
				chk(t,t * 2,t * 3);
			}
}

