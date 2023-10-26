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
#include<iostream>
using namespace std;

char s[100010];
int main()
{
#ifdef llt
	freopen("2.in","r",stdin);
	freopen("2.out","w",stdout);
#endif
	cin.getline(s + 1,300);
	int len = strlen(s + 1);
	bool flag = false;
	for(int i = 1;i <= len;i++)
	{
		if(s[i] == ' ')
		{
			if(!flag)
				flag = true,putchar(' ');
		}
		else if(s[i] == '\r' || s[i] == '\n')
			break;
		else
			putchar(s[i]),flag = false;

	}
}

