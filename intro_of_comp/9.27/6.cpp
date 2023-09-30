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

int ans[1010],cnt;

int main()
{
#ifdef llt
	freopen("6.in","r",stdin);
	freopen("6.out","w",stdout);
#endif
	for(int i = 1;i <= 9;i++)
		for(int j = 0;j <= 9;j++)
			for(int k = 0;k <= 9;k++)
			{
				int t = i * 100 + j * 10 + k;
				if(t == i * i * i + j * j * j + k * k * k)
					ans[++cnt] = t;
			}
	for(int i = 1;i < cnt;i++)
		printf("%d\n",ans[i]);
	printf("%d",ans[cnt]);
}

