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

int a[100][100];
int main()
{
#ifdef llt
	freopen("20.in","r",stdin);
	freopen("20.out","w",stdout);
#endif
	int n,cntn;
	scanf("%d",&n);cntn = n;
	while(cntn--)
	{
		for(int i = 1;i <= n;i++)
			for(int j = 1;j <= n;j++)
				scanf("%d",&a[i][j]);
		int ans = 0;
		for(int i = 1;i <= n;i++)
		{
			for(int j = 1;j <= n;j++)
			{
				int min1 = 10000000;
				if(j == 1 || j > i)
					for(int k = 1;k <= n;k++)
					{
						if((j != 1 && j <= i) || (k != 1 && k <= i))
							continue;
						min1 = min(min1,a[j][k]);
					}
				if(j == 1 || j > i)
					for(int k = 1;k <= n;k++)
					{
						if((j != 1 && j <= i) || (k != 1 && k <= i))
							continue;
						a[j][k] -= min1;
					}
			}
			for(int j = 1;j <= n;j++)
			{
				int min1 = 10000000;
				if(j == 1 || j > i)
					for(int k = 1;k <= n;k++)
					{
						if((j != 1 && j <= i) || (k != 1 && k <= i))
							continue;	
						min1 = min(min1,a[k][j]);
					}
				if(j == 1 || j > i)
					for(int k = 1;k <= n;k++)
					{
						if((j != 1 && j <= i) || (k != 1 && k <= i))
							continue;
						a[k][j] -= min1;
					}
			}
			ans += a[i + 1][i + 1];
		}
		printf("%d\n",ans);
	}
}

