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
	freopen("1.in","r",stdin);
	freopen("1.out","w",stdout);
#endif
	double m;
	scanf("%lf",&m);
	double ans = 0;
	if(m / 0.4883 >= 240)
		ans = 240,m -= 240 * 0.4883;
	else
		ans += m / 0.4883,m = 0;
	if(m / 0.5383 + 240 >= 400)
		ans = 400,m -= (400 - 240) * 0.5383;
	else
		ans += m / 0.5383,m = 0;
	ans += m / 0.7883;
	printf("%.2lf",ans);
}

