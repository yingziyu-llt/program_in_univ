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
int poscnt;
int l;
bool chk(int xx,int n)
{
	if(n == 0) return poscnt * 2 == (l + 1) * l / 2;
	if(poscnt * 2 > (l + 1) * l / 2 || (l + n) * (l - n + 1) / 2 - poscnt > (l + 1) * l / 4)
		return false;
	int tmp = xx;
	while(tmp)
	{
		if(tmp & 1) poscnt++;
		tmp = (tmp >> 1);
	}
	tmp = xx;
	int gen = 0;
	for(int i = 1;i <= n;i++)
		gen = gen | ((!(((tmp >> i) & 1) ^ ((tmp >> (i - 1)) & 1))) << (i - 1));
	return chk(gen,n - 1);
}

int main()
{
#ifdef llt
	freopen("OpenJudge2990.in","r",stdin);
	freopen("OpenJudge2990.out","w",stdout);
#endif
	while(1)
	{
		int n;
		scanf("%d",&n);
		if(n == 0) break;
		int ans = 0;
		l = n;
		for(int i = 0;i < (1 << n);i++)
		{
			poscnt = 0;
			if(chk(i,n - 1))
				ans++;
		}
		printf("%d\n",ans);
	}
}

