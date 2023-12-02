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

int ans[10000],cnt;
bool chk1(int n)
{
	int cpn = n;
	int xx = 0;
	while(n != 0)
	{
		xx = n % 10 + xx * 10;
		n /= 10;
	}
	if(cpn == xx)
		return 1;
	else return 0;
}
bool chk2(int n)
{
	for(int i = 2;i * i <= n;i++)
		if(n % i == 0)
			return false;
	return true;
}
int main()
{
#ifdef llt
	freopen("2.in","r",stdin);
	freopen("2.out","w",stdout);
#endif
	int n,m;
	scanf("%d%d",&n,&m);
	for(int i = n;i <= m;i++)
		if(chk1(i))
			if(chk2(i))
				ans[++cnt] = i;
	if(cnt == 0)
	{
		printf("no");
		return 0;
	}
	for(int i = 1;i < cnt;i++)
		printf("%d,",ans[i]);
	printf("%d",ans[cnt]);
}

