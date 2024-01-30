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


int main()
{
#ifdef llt
	freopen("3.in","r",stdin);
	freopen("3.out","w",stdout);
#endif
	int tot = 0,now = 0;
	int n,ans = 0;
	scanf("%d",&n);
	for(int i = 1;i <= n;i++)
	{
		int t;
		scanf("%d",&t);
		ans += abs(t - tot);
		t -= tot;
		now += t;tot += now + t;
	}
	printf("%d",ans);
	return 0;
}

