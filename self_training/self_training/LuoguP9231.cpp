/*
 * Copyright (C) 2024 Letian Lin <yingziyu-Lin@outlook.com>
 *
 * Distributed under terms of the GNU AGPLv3+ license.
 * 
 * Run for our better future!
 */

#include<stdio.h>
#include<algorithm>
#include<string.h>
#include<math.h>
using namespace std;


int main()
{
#ifdef llt
	freopen("LuoguP9231.in","r",stdin);
	freopen("LuoguP9231.out","w",stdout);
#endif
	int l,r;
	scanf("%d%d",&l,&r);
	int ans = 0;
	ans = (r + 1) / 2 - l / 2 + r / 4 - (l - 1) / 4;
	printf("%d",ans);
}

