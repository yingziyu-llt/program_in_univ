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


int solve(int p,int q)
{
	if(p == 1 && q == 1) return 1;
	if(p / q)
 		return solve(p - q,q) * 2  + 1;
	else
		return solve(p,q - p) * 2;

}

int main()
{
#ifdef llt
	freopen("5.in","r",stdin);
	freopen("5.out","w",stdout);
#endif
	int T;
	scanf("%d",&T);
	while(T--)
	{
		int a,b;
		scanf("%d%d",&a,&b);
		printf("%d\n",solve(a,b));
	}
}

