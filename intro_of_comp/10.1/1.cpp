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
	freopen("1.in","r",stdin);
	freopen("1.out","w",stdout);
#endif
	int a,b;
	scanf("%d%d",&a,&b);
	for(int i = 0;i <= a;i++)
	{
		if(i * 4 + (a - i) * 2 == b)
		{
			printf("%d %d",a - i,i);
			break;
		}
	}
}

