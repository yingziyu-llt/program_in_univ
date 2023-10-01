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

bool isrun(int year)
{
	return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}
int m[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
int main()
{
#ifdef llt
	freopen("18.in","r",stdin);
	freopen("18.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	printf("%c",isrun(n) ? 'Y' : 'N');
}

