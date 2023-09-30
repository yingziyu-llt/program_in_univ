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
	freopen("11.in","r",stdin);
	freopen("11.out","w",stdout);
#endif
	int a,n;
	scanf("%d%d",&a,&n);
	int tot = 0,tmp = a;
	for(int i = 1;i <= n;i++)
	{
		tot += tmp;
		tmp = tmp * 10 + a;
	}
	printf("%d",tot);
}

