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

int a[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
int main()
{
#ifdef llt
	freopen("16.in","r",stdin);
	freopen("16.out","w",stdout);
#endif
	int N;
	scanf("%d",&N);
	for(int _i = 1;_i <= N;_i++)
	{
		int am,ad,bm,bd,init;
		int d = 0;
		scanf("%d%d%d%d%d",&am,&ad,&init,&bm,&bd);
		for(int i = am;i < bm;i++)
			d += a[i];
		d += bd - ad;
		printf("%d\n",init << d);
	}
	return 0;
}

