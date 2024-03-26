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

int block[700][700],blockLable[700],blockSize;
int a[100010];

int getBlock(int x)
{
	return x / blockSize;
}

void buildBlock(int x)
{
	for(int i = x * blockSize;i <= x * blockSize + blockSize;i++)
		block[x][i - x * blockSize] = a[i];
	sort(block[x],block[x] + blockSize);
}

void change(int l,int r,int c)
{
	for(int i = l;i <= r;i++)
		a[i] += c;
}

int query(int l,int r,int c)
{
	int ans = -1;
	int bloc = getBlock(l);
	for(int i = l;i <= r;i++)
	{
		if(a[i] + blockLable[getBlock(i)] < c)
			ans = max(ans,a[i] + blockLable[bloc]);
	}
	return ans;
}

int main()
{
#ifdef llt
	freopen("LOJ6279.in","r",stdin);
	freopen("LOJ6279.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	blockSize = sqrt(n);
	for(int i = 0;i < n;i++)
		scanf("%d",&a[i]);
	for(int i = 0;i * blockSize < n;i++)
		buildBlock(i);

	for(int i = 1;i <= n;i++)
	{
		int opt,l,r,c;
		scanf("%d%d%d%d",&opt,&l,&r,&c);
		l--,r--;
		if(opt == 0)
		{
			change(l,min(getBlock(l) * blockSize + blockSize - 1,r),c);
			buildBlock(getBlock(l));
			for(int j = getBlock(l) + 1;j < getBlock(r);j++)
				blockLable[j] += c;
			if(getBlock(l) != getBlock(r))
				change(getBlock(r) * blockSize,r,c),buildBlock(getBlock(r));
		}
		else
		{
			int ans = -1;
			ans = max(query(l,min(getBlock(l) * blockSize + blockSize - 1,r),c),ans);
			for(int j = getBlock(l) + 1;j < getBlock(r);j++)
			{
				int xx = lower_bound(block[j],block[j] + blockSize,c - blockLable[j]) - block[j];
				if(block[j][xx] + blockLable[j] >= c)
					continue;
				ans = max(block[j][xx] + blockLable[j],ans);
			}
			if(getBlock(l) != getBlock(r))
				ans = max(query(getBlock(r) * blockSize,r,c),ans);
			printf("%d\n",ans);
		}
	}
}

