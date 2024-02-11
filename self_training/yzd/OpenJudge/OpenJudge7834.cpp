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

long long gcd(long long a,long long b)
{
	return a % b == 0 ? b : gcd(b,a % b);
}

long long a[20];
long long curg[20][20];//current group
int cnt[20];
long long n,ans = 100;

void dfs(long long group,int cur)
{
	if(cur == n + 1)
	{
		ans = min(ans,group);
		return ;
	}
	for(int i = 1;i <= group;i++)
	{
		bool flag = 1;
		for(int j = 1;j <= cnt[i];j++)
			if(gcd(a[cur],curg[i][j]) != 1)
			{
				flag = 0;
				break;
			}
		if(flag)
			curg[i][++cnt[i]] = a[cur],dfs(group,cur + 1),curg[i][cnt[i]] = 1,cnt[i]--;
	}
	curg[group + 1][1] = a[cur];
	cnt[group + 1] = 1;
	dfs(group + 1,cur + 1);
	cnt[group + 1] = 0;
	curg[group + 1][1] = 1;

}

int main()
{
#ifdef llt
	freopen("OpenJudge7834.in","r",stdin);
	freopen("OpenJudge7834.out","w",stdout);
#endif
	scanf("%lld",&n);
	if(n == 1)
	{
		printf("1");
		return 0;
	}
	for(int i = 1;i <= n;i++)
		scanf("%lld",&a[i]);
	curg[1][1] = 1;
	dfs(1,1);
	printf("%lld",ans);
}

