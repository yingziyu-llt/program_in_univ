/*
 * Copyright (C) 2024 Letian Lin <yingziyu-Lin@outlook.com>
 *
 * Distributed under terms of the GNU AGPLv3+ license.
 * 
 * Run for our better future!
 */

#include<stdio.h>
#include<algorithm>
#include<string.h>
#include<math.h>
using namespace std;


long long ksm1(long long a,long long b,long long p)
{
	if(b == 1) return a;
	long long ans = 1;
	long long pow2 = ksm1(a,b / 2,p);
	if(b % 2 == 1)
		ans = pow2 * pow2 % p * a % p;
	else
		ans = pow2 * pow2 % p;
	return ans;
}

long long ksm(long long a,long long b,long long p)
{
	long long ans = 1;
	while(b)
	{
		if(b & 1)
			ans = ans * a % p;
		a = a * a % p;
		b >>= 1;
	}
	return ans;
}

int main()
{
#ifdef llt
	freopen("LuoguP1226.in","r",stdin);
	freopen("LuoguP1226.out","w",stdout);
#endif
	long long a,b,p;
	scanf("%lld%lld%lld",&a,&b,&p);
	printf("%lld^%lld mod %lld=%lld",a,b,p,ksm(a,b,p));
}

