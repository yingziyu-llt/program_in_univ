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
void devide(int &x,int k)
{
	while(x % k == 0)
		x /= k;
}

int main()
{
#ifdef llt
	freopen("3.in","r",stdin);
	freopen("3.out","w",stdout);
#endif
	int a,b;
	scanf("%d,%d",&a,&b);
	for(int i = 2;i <= max(a,b);i++)
	{
		if(a % i == 0)
		{
			if(b % i != 0)
			{
				printf("NO");
				return 0;
			}
			devide(a,i),devide(b,i);
		}
	}
	if(b != 1) printf("NO");
	else
		printf("YES");
}

