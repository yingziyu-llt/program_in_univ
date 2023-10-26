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

int to(int x,int a)
{
	int ans = 0,tt = 1;
	while(x != 0)
	{
		ans = ans + x % 10 * tt;
		x /= 10;
		tt *= a;
	}
	return ans;
}

bool unable(int x,int a)
{
	while(x != 0)
	{
		if(x % 10 >= a) return true;
		x /= 10;
	}
	return false;
}
int main()
{
#ifdef llt
	freopen("12.in","r",stdin);
	freopen("12.out","w",stdout);
#endif
	int T;
	scanf("%d",&T);
	while(T--)
	{
		int a,b,c;
		scanf("%d%d%d",&a,&b,&c);
		bool flag = false;
		if(a == b && b == c)
		{
			if(a == 1)
			{
				puts("2");
				continue;
			}
			puts("0");
			continue;
		}
		for(int i = 2;i <= max(a,max(b,c)) + 1;i++)
		{
			if(unable(a,i) || unable(b,i) || unable(c,i))
				continue;
			if(to(a,i) * to(b,i) == to(c,i))
			{
				printf("%d\n",i);
				flag = true;
				break;
			}
		}
		if(!flag) puts("0");
	}
}

