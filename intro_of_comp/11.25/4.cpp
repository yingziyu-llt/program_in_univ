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

char s[1000];

bool chk(int a,int b)
{
	while(a < b)
	{
		if(s[a] != s[b])
			return false;
		a++,b--;
	}
	return true;
}

void print(int a,int b)
{
	for(int i = a;i <= b;i++)
		putchar(s[i]);
	puts("");
}

int main()
{
#ifdef llt
	freopen("4.in","r",stdin);
	freopen("4.out","w",stdout);
#endif
	scanf("%s",s);
	int n = strlen(s);
	for(int len = 2;len < n;len++)
		for(int from = 0;from + len <= n;from++)
				if(chk(from,from + len - 1))
					print(from,from + len - 1);
}

