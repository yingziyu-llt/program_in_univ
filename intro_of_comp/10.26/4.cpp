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

int a[10010],b[10010];
int main()
{
#ifdef llt
	freopen("4.in","r",stdin);
	freopen("4.out","w",stdout);
#endif
	int n,k,h;
	scanf("%d%d%d",&n,&k,&h);
	for(int i = 1;i <= n;i++)
		scanf("%d",&a[i]);
	for(int i = 1;i <= n + 1;i++)
		scanf("%d",&b[i]);
	long long pre_people = h / k;
	long long ans = 0,cnt = n;
	for(int i = 1;i <= n;i++)
	{
		if(a[i] <= pre_people)
			ans += (a[i] - a[i - 1]) * b[i];
		else
		{
			ans += (pre_people - a[i - 1]) * b[i];
			cnt = i;
			break;
		}
	}
	if(a[n] <= pre_people)
		ans += (pre_people - a[n]) * b[n + 1];
	ans *= k;
	if(h % k)
	{
		if(cnt == n)
			ans += (h % k) * b[n + 1];
		else
		{
			if(h / k + 1 <= a[cnt])
				ans += (h % k) * b[cnt];
			else
				ans += (h % k) * b[cnt + 1];
		}
	}
	printf("%lld",ans);
}

