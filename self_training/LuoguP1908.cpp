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

int a[500010],c[500010];
long long ans = 0;
int read()
{
	int x = 0;
	char c = getchar();
	while(c > '9' || c < '0') c = getchar();
	while(c >= '0' && c <= '9')
		x = (x << 3) + (x << 1) + c - '0',c = getchar();
	return x;
}

void msort(int l,int r)
{
	if(l == r) return ;
	int mid = (l + r) >> 1;
	msort(l,mid),msort(mid + 1,r);
	int i = l,j = mid + 1,k = l;
	while(i <= mid && j <= r)
	{
		if(a[i] <= a[j])
			c[k] = a[i],i++;
		else
			c[k] = a[j],j++,ans += mid - i + 1; 
		k++;
	}
	while(i <= mid)
		c[k++] = a[i++];
	while(j <= r)
		c[k++] = a[j++];
	for(int m = l;m <= r;m++)
		a[m] = c[m];
	return ;
}

int main()
{
#ifdef llt
	freopen("LuoguP1908.in","r",stdin);
	freopen("LuoguP1908.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	for(int i = 1;i <= n;i++)
		a[i] = read();
	msort(1,n);
	printf("%lld",ans);
}

