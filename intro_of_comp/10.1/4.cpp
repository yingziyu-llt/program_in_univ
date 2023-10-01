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
	freopen("5.in","r",stdin);
	freopen("5.out","w",stdout);
#endif
	double tot = 0;
	for(int i = 1;i <= 12;i++)
	{
		double t;
		scanf("%lf",&t);
		tot += t;
	}
	printf("$%.2lf\n",tot / 12.0);
}

