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

int n;
char s[100][100];

bool chk_str(int init,int x,int num)
{
	for(int i = 0;i <= strlen(s[num]) - x;i++)
	{
		bool flag = 1;
		for(int j = 0;j < x;j++)
		{
			if(s[1][init + j] != s[num][i + j])
			{
				flag = false;
				break;
			}
		}
		if(flag) return 1;
		flag = 1;
		for(int j = 1;j <= x;j++)
		{
			if(s[1][init + j - 1] != s[num][i + x - j])
			{
				flag = false;
				break;
			}
		}
		if(flag) return true;
	}
	return false;
}

bool chk(int x)
{
	for(int init = 0;init <= strlen(s[1]) - x;init++)
	{
		bool flag = true;
		for(int j = 2;j <= n;j++)
		{
			if(!chk_str(init,x,j))
			{
				flag = false;
				break;
			}
		}
		if(flag)
			return true;
	}
	return false;
}

int main()
{
#ifdef llt
	freopen("18.in","r",stdin);
	freopen("18.out","w",stdout);
#endif
	int t;
	scanf("%d",&t);
	while(t--)
	{
		scanf("%d",&n);
		int minlen = 10000;
		for(int i = 1;i <= n;i++)
			scanf("%s",s[i]),minlen = min(minlen,(int)strlen(s[i]));
		int curlen = 0,delta = minlen;
		while(delta > 0)
		{
			if(chk(curlen + delta))
				curlen += delta;
			else
				delta /= 2;
		}
		printf("%d\n",curlen);

	}
}

