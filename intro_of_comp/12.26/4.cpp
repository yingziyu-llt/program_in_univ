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

bool isDigit(char c){
	return c >= '0' && c <= '9';
}
int a[256];
int toint(char s[])
{
	int c = 0;
	for(int i = 0;i < (int)strlen(s);i++)
	{
		if(!isDigit(s[i])) break;
		c = c * 10 + s[i] - '0';
	}
	return c;
}

int main()
{
#ifdef llt
	freopen("4.in","r",stdin);
	freopen("4.out","w",stdout);
#endif
	char s[10000];
	scanf("%s",s);
	if(isDigit(s[0]))
	{
		int c = toint(s);
		if(c >= 1000)
		{
			int t = c / 1000;
			for(int i = 1;i <= t;i++)
				putchar('M');
			c %= 1000;
		}
		if(c >= 900)
			c -= 900,putchar('C'),putchar('M');
		if(c >= 500)
			c -= 500,putchar('D');
		if(c >= 400)
			c -= 400,putchar('C'),putchar('D');
		if(c >= 100)
		{
			int t = c / 100;
			for(int i = 1;i <= t;i++)
				putchar('C');
			c %= 100;
		}
		if(c >= 90)
			c -= 90,putchar('X'),putchar('C');
		if(c >= 50)
			c -= 50,putchar('L');
		if(c >= 40)
			c -= 40,putchar('X'),putchar('L');
		if(c >= 10)
		{
			int t = c / 10;
			for(int i = 1;i <= t;i++)
				putchar('X');
			c %= 10;
		}
		if(c >= 9)
			c -= 9,putchar('I'),putchar('X');
		if(c >= 5)
			c -= 5,putchar('V');
		if(c >= 4)
			c -= 4,putchar('I'),putchar('V');
		if(c >= 1)
		{
			int t = c / 1;
			for(int i = 1;i <= t;i++)
				putchar('I');
			c %= 1;
		}
	}
	else
	{
		a['I'] = 1,a['V'] = 5,a['X'] = 10,a['L'] = 50,a['C'] = 100,a['D'] = 500,a['M'] = 1000;
		int ans = 0;
		for(int i = 0;i < (int)strlen(s);i++)
		{
			if(a[s[i]] < a[s[i + 1]] && i != strlen(s) - 1)
				ans -= a[s[i]];
			else
				ans += a[s[i]];
		}
		printf("%d",ans);
	}
}

