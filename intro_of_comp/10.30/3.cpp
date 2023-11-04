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
	freopen("3.in","r",stdin);
	freopen("3.out","w",stdout);
#endif

	int m,n;
	scanf("%d%d",&m,&n);
	int tot = 100 * m + 50 * n;
	int ans = 0;
	for(int i = 1;i * 5 < tot;i++)
		ans += floor((double)(tot - i * 5 - 1) / 2);
	printf("%d",ans);
}

