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

int read()
{
	int c = getchar();
	if(c == -1) return -1;
	while(c < '0' || c > '9')
	{
		c = getchar();
		if(c == -1)
			return -1;
	}
	int ans = 0;
	while(c >= '0' && c <= '9')
		ans = (ans << 3) + (ans << 1) + c - '0',c = getchar();
	return ans;
}

int main()
{
#ifdef llt
	freopen("14.in","r",stdin);
	freopen("14.out","w",stdout);
#endif
	int maxn = 0,sec = 0;
	while(1)
	{
		int cc = read();
		if(cc == -1)
			break;
		if(cc > maxn)
			sec = maxn,maxn = cc;
		else if(cc > sec && cc != maxn)
			sec = cc;
	}
	if(maxn == sec || sec == 0)
		printf("No");
	else
		printf("%d",sec);
}

