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
	freopen("20.in","r",stdin);
	freopen("20.out","w",stdout);
#endif
	int n,x,y;
	scanf("%d%d%d",&n,&x,&y);
	printf("%d",max(n - (int)ceil((double)y / (double)x),0));
}

