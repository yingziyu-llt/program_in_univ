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

int n,k;
int check(int mm)
{
	for(int i = 1;i < n;i++)
	{
		if(mm % (n - 1)) return 0;
		mm = mm / (n - 1) * n + k;
	}
	return mm;
}
int main()
{
#ifdef llt
	freopen("15.in","r",stdin);
	freopen("15.out","w",stdout);
#endif
	scanf("%d%d",&n,&k);
	for(int i = 1; ;i++)
	{
		int m = n * i + k;
		if(check(m))
		{
			printf("%d",check(m));
			break;
		}
	}
}

