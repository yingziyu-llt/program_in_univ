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

bool chk(int k)
{
	return (k % (k / 10 + k % 10) == 0);
}

int main()
{
#ifdef llt
	freopen("1.in","r",stdin);
	freopen("1.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	for(int i = 10;i <= n;i++)
		if(chk(i))
			printf("%d\n",i);
}

