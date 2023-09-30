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


int main()
{
#ifdef llt
	freopen("10.in","r",stdin);
	freopen("10.out","w",stdout);
#endif
	int m,n;
	scanf("%d%d",&m,&n);
	int cnt = 0;
	for(int i = 1;i <= n;i++)
	{
		int t;
		scanf("%d",&t);
		if(t > m)
			cnt++;
		else
			m -= t;
	}
	printf("%d",cnt);
}

