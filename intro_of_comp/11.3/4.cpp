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
	freopen("4.in","r",stdin);
	freopen("4.out","w",stdout);
#endif
	int p,e,i,d;
	int cnt = 0;
	while(1)
	{
		cnt++;
		scanf("%d%d%d%d",&p,&e,&i,&d);
		if(p == -1) break;
		p %= 23,e %= 28,i %= 33;
		for(int date = d + 1;;date++)
		{
			if(date % 23 == p && date % 28 == e && date % 33 == i)
			{
				printf("Case %d: the next triple peak occurs in %d days.\n",cnt,date -  d);
				break;
			}
		}
	}
}

