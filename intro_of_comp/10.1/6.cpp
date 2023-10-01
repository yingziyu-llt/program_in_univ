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
	freopen("7.in","r",stdin);
	freopen("7.out","w",stdout);
#endif
	int a,b;
	int type;
	char c;
	while(~scanf("%d%d %c%d",&a,&b,&c,&type))
	{
		if(a == 0) break;
		swap(a,b);
		if(type == 1)
			for(int i = 1;i <= b;i++)
			{
				for(int j = 1;j <= a;j++)
					putchar(c);
				puts("");
			}
		else
		{
			for(int j = 1;j <= a;j++)
				putchar(c);
			puts("");
			for(int i = 2;i < b;i++)
			{
				putchar(c);
				for(int j = 2;j < a;j++)
					putchar(' ');
				putchar(c);
				puts("");
			}
			for(int j = 1;j <= a;j++)
				putchar(c);
			puts("");
		}
	}
}

