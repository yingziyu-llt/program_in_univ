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


pair <int,double> my_sqrt(double a)
{
	double res = 1.0;
	int cnt = 0;
	while(fabs(1.0/2.0*(res + a / res) - res) >= 1e-6)
	{
		res = res + a / res;
		res = res * 0.5;
		cnt++;
	}
	return make_pair(cnt,res);
}

int main()
{
#ifdef llt
	freopen("4.in","r",stdin);
	freopen("4.out","w",stdout);
#endif
	double a;
	while(~scanf("%lf",&a))
	{
		pair <int,double> res;
		res = my_sqrt(a);
		printf("%d %.2lf\n",res.first + 1,res.second);
	}
}	
