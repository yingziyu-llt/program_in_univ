/*
 * Copyright (C) 2023 Letian Lin <yingziyu-Lin@outlook.com>
 *
 * Distributed under terms of the GNU AGPLv3+ license.
 * 
 * Run for our better future
 */

#include <iterator>
#include<stdio.h>
#include<algorithm>
#include<string.h>
#include<math.h>
using namespace std;

bool isp[5100];
int prime[5100],cnt;

void gen_prime(int t)
{
	memset(isp,1,sizeof(isp));
	for(int i = 2;i <= t;i++)
		if(isp[i])
			for(int j = 2;j * i <= t;j++)
				isp[i * j] = 0;
	for(int i = 2;i <= t;i++)
		if(isp[i])
			prime[++cnt] = i;
}

int main()
{
#ifdef llt
	freopen("2.in","r",stdin);
	freopen("2.out","w",stdout);
#endif
	int m,n;
	scanf("%d%d",&m,&n);
	gen_prime(n);
	for(int i = m;i <= n;i++)
	{
		int maxp = lower_bound(prime + 1,prime + 1 + cnt,i) - prime;
		if(maxp > cnt) maxp--;
		for(int j = maxp;j >= 1;j--)
			if(i % prime[j] == 0)	
			{
				if(i == n)printf("%d",prime[j]);
				else      printf("%d,",prime[j]);
				break;
			}
	}
}

