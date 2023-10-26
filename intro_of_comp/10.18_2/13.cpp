/*
 * Copyright (C) 2023 Letian Lin <yingziyu-Lin@outlook.com>
 *
 * Distributed under terms of the GNU AGPLv3+ license.
 * 
 * Run for our better future
 */

#include <iterator>
#include<stdio.h>
#include<algorithm>
#include<string.h>
#include<math.h>
using namespace std;

int mp[110][110];

int main()
{
#ifdef llt
	freopen("13.in","r",stdin);
	freopen("13.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	memset(mp,0x3f,sizeof(mp));
	for(int i = 1;i <= n;i++)
		for(int j = 1;j <= n;j++)
			scanf("%d",&mp[i][j]);
	int c = 0,s = 0;
	for(int i = 1;i <= n;i++)
		for(int j = 1;j <= n;j++)
		{
			if(mp[i][j] <= 50)
			{
				s++;
				if(mp[i - 1][j] > 50 || mp[i + 1][j] > 50 || mp[i][j - 1] > 50 || mp[i][j + 1] > 50)
					c++;
			}
		}
	printf("%d %d",s,c);
}

