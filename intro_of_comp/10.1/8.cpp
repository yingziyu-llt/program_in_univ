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
	freopen("8.in","r",stdin);
	freopen("8.out","w",stdout);
#endif
	int a,b,c,d;
	scanf("%d%d%d%d",&a,&b,&c,&d);
	double tot = a / 100.0 * 110 + b / 100.0 * 120 + c / 100.0 * 2250 + d / 100.0 * 880;
	if(tot < 600)
		printf("UNDER");
	else if(tot > 1000)
		printf("OVER");
	else printf("FINE");
}

