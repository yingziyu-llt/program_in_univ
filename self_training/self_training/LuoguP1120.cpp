/*
 * Copyright (C) 2023 Letian Lin <yingziyu-Lin@outlook.com>
 *
 * Distributed under terms of the GNU AGPLv3+ license.
 * 
 * Run for our better future
 */

#include <functional>
#include<stdio.h>
#include<algorithm>
#include<string.h>
#include<math.h>
using namespace std;

int a[70];
int nxt[70];
bool used[70];
int acnt,n;
bool succ = 0;
int len = 0;
int tot = 0;
int m;

void dfs(int num,int cur,int ret)
{
	if(ret == 0)
	{
		if(num == m)
		{
			succ = 1;
			return ;
		}
		dfs(num + 1,1,len);
		return ;
	}
	int l = cur,r = n;
	while(l < r)
	{
		int mid = (l + r) >> 1;
		if(a[mid] > ret) l = mid + 1;
		else r = mid;
	}
	for(int i = l;i <= n;i++)
	{
		if(used[i]) continue;
		used[i] = 1;
		dfs(num,i + 1,ret - a[i]);
		if(succ) return ;
		used[i] = 0;
		if(a[i] == ret || ret == len) return ;
		i = nxt[i];
		if(i == n) return ;

	}
}

int main()
{
#ifdef llt
	freopen("LuoguP1120.in","r",stdin);
	freopen("LuoguP1120.out","w",stdout);
#endif
	scanf("%d",&n);
	for(int i = 1;i <= n;i++)
	{
		int t;
		scanf("%d",&t);
		if(t <= 50)
			a[++acnt] = t,tot += t;
	}
	n = acnt;
	nxt[n] = n;
	sort(a + 1,a + 1 + n,greater<int>());
	for(int i = n - 1;i > 0;i--)
	{
		if(a[i] == a[i + 1]) nxt[i] = nxt[i + 1];
		else nxt[i] = i;
	}
	used[1] = 1;
	for(int i = a[1];i <= tot / 2;i++)
	{
		if(tot % i) continue;
		len = i;
		m = tot / i;
		dfs(1,0,i - a[1]);
		if(succ)
		{
			printf("%d",i);
			return 0;
		}
	}
	printf("%d",tot);
}

