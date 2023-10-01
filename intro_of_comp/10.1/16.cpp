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
	freopen("16.in","r",stdin);
	freopen("16.out","w",stdout);
#endif
	int n;
	double h;
	scanf("%lf%d",&h,&n);
	printf("%.4lf %.4lf",h * (1 - 1 / (double)(1 << (n - 1))) * 2 + h,h * (1.0 / (1 << n)));
}

