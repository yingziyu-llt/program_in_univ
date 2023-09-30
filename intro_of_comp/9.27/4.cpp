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
	freopen("4.in","r",stdin);
	freopen("4.out","w",stdout);
#endif
	int a,b,c;
	scanf("%d,%d,%d",&a,&b,&c);
	if(a + b == c)
		putchar('+');
	else if(a - b == c)
		putchar('-');
	else if(a * b == c)
		putchar('*');
	else
	{
		if(b != 0)
		{
			if(a / b == c)
				putchar('/');
			else if(a % b == c)
				putchar('%');
			else
				puts("error");
		}
		else puts("error");
	}
}

