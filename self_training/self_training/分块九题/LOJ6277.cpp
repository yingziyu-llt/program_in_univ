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

int n;
int blockSize;
int a[50100];
int block[5000];

void init()
{
	for(int i = 0;i * blockSize < n;i++)
		for(int j = 0;j < blockSize;j++)
			block[i] += a[i * blockSize + j];
	return ;
}

int get_block(int x)
{
	return x / blockSize;
}

int main()
{
#ifdef llt
	freopen("LOJ6277.in","r",stdin);
	freopen("LOJ6277.out","w",stdout);
#endif
	scanf("%d",&n);
	blockSize = sqrt(n);
	for(int i = 0;i < n;i++)
		scanf("%d",&a[i]);
	for(int i = 1;i <= n;i++)
	{
		int opt,l,r,c;
		scanf("%d%d%d%d",&opt,&l,&r,&c);
		l--,r--;
		if(opt == 0)
		{
			for(int j = get_block(l) + 1;j < get_block(r);j++)
				block[j] += c;
			for(int j = l;j <= min((get_block(l) + 1) * blockSize - 1,r);j++)
				a[j] += c;
			if(get_block(l) != get_block(r))
				for(int j = get_block(r) * blockSize;j <= r;j++)
					a[j] += c;
		}
		else
			printf("%d\n",a[r] + block[r / blockSize]);
	}
}

