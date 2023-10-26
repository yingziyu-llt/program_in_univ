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
	freopen("C.in","r",stdin);
	freopen("C.out","w",stdout);
#endif
	int t;
	scanf("%d",&t);
	while(t--)
	{
		int n,m;
		scanf("%d%d",&n,&m);
		int kk = m / __gcd(n,m);
		while(kk != 1)
		{
			if(kk & 1)
			{
				printf("-1\n");
				break;
			}
			kk /= 2;
		}
		if(kk == 1)
		{
			int ret = n % m;
			if(ret == 0) printf("0\n");
			else
			{
				int gcd = __gcd(ret,m),group = m / gcd;
				printf("%d\n",(gcd * (ret / gcd)) * (group - 1));
			}
		}
	}
}

