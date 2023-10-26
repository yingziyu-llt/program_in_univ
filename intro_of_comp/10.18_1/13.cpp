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
	int res = 0;
	char c = 0;
	while(c > '9' || c < '0')
	{
		if(c == -1) return -1;
		c = getchar();
	}
	while(c >= '0' && c <= '9')
	{
		res = (res << 3) + (res << 1) + c - '0';
		c = getchar();
	}
	if(c == EOF)
		return -1;
	return res;
}
int a[10000];
int main()
{
#ifdef llt
	freopen("13.in","r",stdin);
	freopen("13.out","w",stdout);
#endif
	int k,cnt = 0;
	while((k = read()) != -1)
		a[++cnt] = k;
	sort(a + 1,a + 1 + cnt);
	for(int i = 1;i < cnt;i++)
		printf("%d,",a[i]);
	printf("%d",a[cnt]);
}

