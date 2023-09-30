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
	int n;
	scanf("%d",&n);
	if(n % 3 == 0 || n % 10 == 3 || n / 10 % 10 == 3)
		printf("TRUE");
	else
		printf("FALSE");
	return 0;
}

