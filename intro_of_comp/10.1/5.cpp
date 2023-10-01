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
	freopen("6.in","r",stdin);
	freopen("6.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	double ans = 0;
	for(int i = 1;i <= n;i++)
		ans += 1.0 / (double)i;
	printf("%.9lf",ans);
}

