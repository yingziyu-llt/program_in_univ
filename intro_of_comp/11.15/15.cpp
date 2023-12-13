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

int reverse(int n)
{
	int res = 0;
	while(n != 0)
		res = res * 10 + n % 10,n /= 10;
	return res;
}

int main()
{
#ifdef llt
	freopen("15.in","r",stdin);
	freopen("15.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	for(int i = 1;i <= 7;i++)
	{
		int rev = reverse(n);
		if(n == rev)
		{
			putchar('Y');
			return 0;
		}
		printf("%d+%d=%d\n",n,rev,n + rev);
		n = n + rev;
	}
	int rev = reverse(n);
	if(n != rev)
		printf("N");
	else
		printf("Y");
	return 0;
}

