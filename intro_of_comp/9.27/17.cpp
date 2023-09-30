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
	freopen("17.in","r",stdin);
	freopen("17.out","w",stdout);
#endif
	int a = 0,b = 0;
	char c;
	while(c = getchar())
	{
		if(c > '9' || c < '0')
			break;
		a = (a << 3) + (a << 1) + c - '0'; 
	}
	printf("%c ",c);
	while(c = getchar())
	{
		if(c > '9' || c < '0')
			break;
		b = (b << 3) + (b << 1) + c - '0';
	}
	printf("%d %d",a,b);
}

