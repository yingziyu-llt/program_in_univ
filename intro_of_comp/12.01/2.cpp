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
	freopen("2.in","r",stdin);
	freopen("2.out","w",stdout);
#endif
	int n,k;
	scanf("%d%d",&n,&k);
	int upper = pow(k,n);
	for(int i = 0;i < upper;i++)
	{
		int cp_upper = upper;
		while(upper >= k)
		{
			upper /= k;
			printf("%d",i / upper % k);
		}
		upper = cp_upper;
		if(i % 6 == 5 || i == upper - 1)
			printf("\n");
		else
			printf(",");
	}

}

