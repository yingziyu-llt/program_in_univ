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
	freopen("16.in","r",stdin);
	freopen("16.out","w",stdout);
#endif
	int m;
	scanf("%d",&m);
	while(m--)
	{
		char s[100];
		scanf("%s",s);
		int len = strlen(s);
		if(strcmp(s + len - 2,"er") == 0 || strcmp(s + len - 2,"ly") == 0)
		{
			for(int i = 0;i < len - 2;i++)
				putchar(s[i]);
		}
		else
			for(int i = 0;i < len - 3;i++)
				putchar(s[i]);
		puts("");
	}
}

