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

bool isprime[100010];
int prime[100010],cnt;
int main()
{
#ifdef llt
	freopen("20.in","r",stdin);
	freopen("20.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	memset(isprime,1,sizeof(isprime));
	for(int i = 2;i <= n;i++)
		if(isprime[i])
			for(int j = 2;i * j <= n;j++)
				isprime[i * j] = 0;
	for(int i = 2;i <= n;i++)
		if(isprime[i])
			prime[++cnt] = i;
	for(int _i = 6;_i <= n;_i += 2)
	{
		for(int i = 1;i <= cnt;i++)
			if(isprime[_i - prime[i]] == 1)
			{
				printf("%d=%d+%d\n",_i,prime[i],_i - prime[i]);
				break;
			}
	}
}

