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

int gcd(int a,int b)
{
	return b == 0 ? a : gcd(b,a % b);
}

int main()
{
#ifdef llt
	freopen("5.in","r",stdin);
	freopen("5.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	int a,b;
	int lcm;
	scanf("%d",&a);
	lcm = a;
	for(int i = 1;i < n;i++)
	{
		scanf("%d",&b);
		a = gcd(a,b);
		lcm = lcm * b / gcd(lcm,b);
	}
	printf("%d %d",a,lcm);
}

