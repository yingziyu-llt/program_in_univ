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
	freopen("13.in","r",stdin);
	freopen("13.out","w",stdout);
#endif
	int m,k;
	scanf("%d%d",&m,&k);
	int tot = 0;
	if(m % 19 != 0)
	{
		printf("NO");
		return 0;
	}
	while(m != 0)
	{
		tot += (m % 10 == 3);
		m /= 10;
	}
	if(tot == k)
		printf("YES");
	else
		printf("NO");
}

