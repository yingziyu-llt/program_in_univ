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

pair<int,int> a[100010],b[100010];
int c[100010],x[100010];
int ans;

void msort(int l,int r)
{
	if(l == r) return ;
	int mid = (l + r) >> 1;
	msort(l,mid),msort(mid + 1,r);
	int i = l,j = mid + 1,k = l;
	while(i <= mid && j <= r)
	{
		if(x[i] <= x[j])
			c[k++] = x[i++];
		else
			ans += mid - i + 1,c[k++] = x[j++],ans %= (int)(1e8 - 3);
	}
	while(i <= mid)
		c[k++] = x[i++];
	while(j <= r)
		c[k++] = x[j++];
	for(int i = l;i <= r;i++)
		x[i] = c[i];
	return ;
}

int main()
{
#ifdef llt
	freopen("LuoguP1966.in","r",stdin);
	freopen("LuoguP1966.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	for(int i = 1;i <= n;i++)
		scanf("%d",&a[i].first),a[i].second = i;
	for(int i = 1;i <= n;i++)
		scanf("%d",&b[i].first),b[i].second = i;
	sort(a + 1,a + 1 + n);
	sort(b + 1,b + 1 + n);
	for(int i = 1;i <= n;i++)
		x[b[i].second] = a[i].second;
	msort(1,n);
	printf("%d",ans);
}

