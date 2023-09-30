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

int dp[110][110];

struct mons
{
	int hp,a,d;
}a[100010];

int hurt(int A,int D,int mA,int mD,int mHP) //calcuate the hurt hero suffered m -> monster
{
	return (int)ceil(mHP / (float)(A - mD)) * max((mA - D),0);
}

int main()
{
#ifdef llt
	freopen("12.in","r",stdin);
	freopen("12.out","w",stdout);
#endif
	int A,D;//Attect Defend
	int dA,dD;
	scanf("%d%d",&A,&D);
	scanf("%d%d",&dA,&dD);
	int n;
	scanf("%d",&n);
	for(int i = 0;i < n;i++)
		scanf("%d%d%d",&a[i].hp,&a[i].a,&a[i].d);
	bool flag = 0;
	int cur = A;
	for(int i = 0;i < n;i++)
	{
		if(cur <= a[i].d)
		{
			flag = 1;
			break;
		}
		cur += dA;
	}
	if(flag)
	{
		printf("impossible");
		return 0;
	}
	memset(dp,0x3f,sizeof(dp));
	dp[0][0] = 0;
	for(int i = 0;i < n;i++)
	{
		for(int j = i;j >= 0;j--)
		{
			int k = i - j;
			if(A + j * dA <= a[i].d)
				continue;
			int ht = hurt(A + j * dA,D + k * dD,a[i].a,a[i].d,a[i].hp);
			dp[j + 1][k] = min(dp[j + 1][k],dp[j][k] + ht);
			dp[j][k + 1] = min(dp[j][k + 1],dp[j][k] + ht);
		}
	}
	int ans = 0x3fffffff;
	for(int i = 0;i <= n - 1;i++)
		ans = min(ans,dp[i][n - 1 - i]);
	printf("%d",ans + 1);
}

