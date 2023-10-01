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
	freopen("14.in","r",stdin);
	freopen("14.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	if(n == 0)
		printf("0");
	while(n != 0)
	{
		printf("%d",n%10);
		n /= 10;
	}
}

