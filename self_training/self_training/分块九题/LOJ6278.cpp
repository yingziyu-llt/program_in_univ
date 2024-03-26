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

int block[300][300],a[50010];
int block_lab[300];
int blockSize;

int get_block(int x)
{
	return x / blockSize;
}

int main()
{
#ifdef llt
	freopen("LOJ6278.in","r",stdin);
	freopen("LOJ6278.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	for(int i = 0;i < n;i++)
		scanf("%d",&a[i]);
	blockSize = sqrt(n);
	for(int i = 0;i < blockSize;i++)
		for(int j = 0;j < blockSize;j++)
			block[i][j] = a[i * blockSize + j];
	for(int i = 0;i < blockSize;i++)
		sort(block[i],block[i] + blockSize);
	for(int i = 0;i < n;i++)
	{
		int opt,l,r,c;
		scanf("%d%d%d%d",&opt,&l,&r,&c);
		l--,r--;
		if(opt == 0)
		{
			for(int j = get_block(l) + 1;j < get_block(r);j++)
				block_lab[j] += c;
			for(int j = l;j < min((get_block(l) + 1) * blockSize,r + 1);j++)
				a[j] += c;
			for(int j = get_block(l) * blockSize;j <= get_block(l) * blockSize + blockSize;j++)
				block[get_block(l)][j - get_block(l) * blockSize] = a[j];
			sort(block[get_block(l)],block[get_block(l)] + blockSize);
			if(get_block(l) != get_block(r))
			{
				for(int j = get_block(r) * blockSize;j <= r;j++)
					a[j] += c;
				for(int j = get_block(r) * blockSize;j <= get_block(r) * blockSize + blockSize;j++)
					block[get_block(r)][j - get_block(r) * blockSize] = a[j];
				sort(block[get_block(r)],block[get_block(r)] + blockSize);
			}
		}
		else
		{
			int ans = 0;
			for(int j = get_block(l) + 1;j < get_block(r);j++)
			{
				int q = c * c - block_lab[j];
				ans += lower_bound(block[j],block[j] + blockSize,q) - block[j];
			}
			for(int j = l;j < min((get_block(l) + 1) * blockSize,r + 1);j++)
				if(a[j] + block_lab[get_block(j)] < c * c)
					ans++;
			if(get_block(l) != get_block(r))
				for(int j = get_block(r) * blockSize;j <= r;j++)
					if(a[j] + block_lab[get_block(j)] < c * c)
						ans++;
			printf("%d\n",ans);
		}
	}
}

