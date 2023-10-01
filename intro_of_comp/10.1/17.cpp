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

bool isprime(int k)
{
	for(int i = 2;i * i <= k;i++)
		if(k % i == 0)
			return 0;
	return 1;
}

int main()
{
#ifdef llt
	freopen("17.in","r",stdin);
	freopen("17.out","w",stdout);
#endif
	int m;
	scanf("%d",&m);
	for(int i = m - 2;i >= 1;i--)
	{
		if(isprime(i) && isprime(i + 2))
		{
			printf("%d %d",i,i + 2);
			break;
		}
	}
}

