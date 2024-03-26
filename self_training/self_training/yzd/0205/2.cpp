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

pair<int,int> a[50100];

int main()
{
#ifdef llt
	freopen("2.in","r",stdin);
	freopen("2.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	for(int i = 1;i <= n;i++)
		scanf("%d%d",&a[i].first,&a[i].second),a[i].second++,a[i].second *= -1;
	sort(a + 1,a + 1 + n);
	int fail = 0;
	int tail = -a[1].second,head = a[1].first;
	for(int i = 1;i <= n;i++)
	{
		if(a[i].first <= tail)
			tail = max(tail,-a[i].second);
		else
			fail++,tail = -a[i].second,head = a[i].first;
	}
	if(fail == 1)
		printf("%d %d",head,tail);
	else
		printf("No!");
}

