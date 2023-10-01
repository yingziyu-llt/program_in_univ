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
	freopen("13.in","r",stdin);
	freopen("13.out","w",stdout);
#endif
	int by,bm,bd;
	int ey,em,ed;
	int tot = 0;
	scanf("%d%d%d%d%d%d",&by,&bm,&bd,&ey,&em,&ed);
	for(int i = by;i < ey;i++)
	{
		if(isrun(i))
			tot += 366;
		else tot += 365;
	}
	for(int i = 1;i < bm;i++)
	{
		tot -= m[i];
		if(isrun(by) && i == 2) tot -= 1;
	}
	for(int i = 1;i < em;i++)
	{
		tot += m[i];
		if(isrun(ey) && i == 2) tot += 1;
	}
	tot -= bd,tot += ed;
	printf("%d",tot);

}

