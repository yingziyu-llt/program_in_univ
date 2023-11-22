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
#include<stdlib.h>
#include<math.h>
using namespace std;

int ans[100];
void work(int a)
{
	unsigned int x = a;
	for(int i = 0;i <= 31;i++)
		ans[i] = ((x & (1 << i)) > 0);
	for(int i = 31;i >= 0;i--)
		printf("%d",ans[i]);
	puts("");
}

bool is_alpha(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int main()
{
#ifdef llt
	freopen("2.in","r",stdin);
	freopen("2.out","w",stdout);
#endif
	while(1)
	{
		char ch = getchar();
		if(ch == -1 || is_alpha(ch))
			break;
		int sgn = 1;
		while(ch < '0' || ch > '9')
		{
			if(ch == '-')
				sgn = -sgn;
			ch = getchar();
			if(ch == -1 || is_alpha(ch))
				break;
		}
		if(ch == -1 || is_alpha(ch))
			break;
		int a = 0;
		while(ch >= '0' && ch <= '9')
			a = a * 10 + ch - '0',ch = getchar();
		a *= sgn;
		work(a);
		if(ch == -1 || is_alpha(ch))
			break;
	}
}

