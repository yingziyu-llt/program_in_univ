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
	freopen("A.in","r",stdin);
	freopen("A.out","w",stdout);
#endif
	int t;
	scanf("%d",&t);
	while(t--)
	{
		long long a,b,n;
		long long ans = 0;
		scanf("%lld%lld%lld",&a,&b,&n);
		for(int i = 1;i <= n;i++)
		{
			long long tmp = 0;
			scanf("%lld",&tmp);
			ans += min(tmp,a - 1);
		}
		ans += b;
		printf("%lld\n",ans);
	}
}

