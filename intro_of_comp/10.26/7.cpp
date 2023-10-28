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

int a[1100];

int main()
{
#ifdef llt
	freopen("7.in","r",stdin);
	freopen("7.out","w",stdout);
#endif
	while(1)
	{
		int r,n;
		scanf("%d%d",&r,&n);
		if(r == -1 && n == -1)
			break;
		for(int i = 1;i <= n;i++)
			scanf("%d",a + i);
		sort(a + 1,a + 1 + n);
		int ans = 0;
		int put_pos = a[1] - r - 1;
		for(int i = 1;i <= n;i++)
		{
			if(a[i] > put_pos + r)
			{
				bool flag = 1;
				for(int j = i + 1;j <= n;j++)
					if(a[j] - r > a[i])
					{
						put_pos = a[j - 1];
						ans++;
						flag = 0;
						break;
					}
				if(flag)
					put_pos = a[n],ans++;
			}
		}
		printf("%d\n",ans);
	}
}

