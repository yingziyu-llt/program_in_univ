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
long long f(int k,int n);
long long findmin(int k,int n);
long long dp[1000][1000];
long long f(int k,int n)
{
	if(k == 3 && n > 63) return 0x7fffffffffffff;
	if(k == 3) return pow(2,n) - 1ll;
	if(n == 1) return 1;
	return dp[k][n] = dp[k][n] == 0 ? findmin(k,n) : dp[k][n];
}

long long findmin(int k,int n)
{
	long long ans = 0x7ffffffffffffff;
	for(int i = 1;i < n;i++)
		ans = min((long long)ans,(2ll *(long long) f(k,i) + f(k - 1,n - i)) < 0 ? ans : (2ll *(long long) f(k,i) + f(k - 1,n - i)));
	return ans;
}


int main()
{
#ifdef llt
	freopen("1.in","r",stdin);
	freopen("1.out","w",stdout);
#endif
	int k,n;
	scanf("%d%d",&k,&n);
	printf("%lld",f(k,n));
}

