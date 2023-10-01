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
	freopen("12.in","r",stdin);
	freopen("12.out","w",stdout);
#endif
	int a;
	scanf("%d",&a);
	int tot = 0,cp_a = a;
	for(int i = 2;i <= a;i++)
		if(a % i == 0)
			a /= i,tot += i,i--;
	if(__gcd(cp_a,tot) != 1)
		printf("no");
	else
		printf("yes");
}

