/*
 * =template=.cpp
 * Copyright (C) 2023 Letian Lin <yingziyu-Lin@outlook.com>
 *
 * Distributed under terms of the GNU AGPLv3+ license.
 */

#include<stdio.h>
#include<algorithm>
#include<string.h>
#include<math.h>

int main()
{
#ifdef llt
	freopen("39.in","r",stdin);
	freopen("39.out","w",stdout);
#endif
	auto n = 0;
	scanf("%d",&n);
	printf("%d",1<<n);
}

