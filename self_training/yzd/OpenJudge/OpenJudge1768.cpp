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

int a[110][110];
int sum[110][110];

int main()
{
#ifdef llt
	freopen("OpenJudge1768.in","r",stdin);
	freopen("OpenJudge1768.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	for(int i = 1;i <= n;i++)
		for(int j = 1;j <= n;j++)
			scanf("%d",&a[i][j]);
	for(int i = 1;i <= n;i++)
		for(int j = 1;j <= n;j++)
			sum[i][j] = sum[i][j - 1] + sum[i - 1][j] - sum[i - 1][j - 1] + a[i][j];
	int ans = a[1][1];
	for(int x1 = 1;x1 <= n;x1++)
		for(int y1 = 1;y1 <= n;y1++)
			for(int x2 = 1;x2 <= x1;x2++)
				for(int y2 = 1;y2 <= y1;y2++)
					ans = max(ans,sum[x1][y1] - sum[x2 - 1][y1] - sum[x1][y2 - 1] + sum[x2 - 1][y2 - 1]);
	printf("%d",ans);
}

