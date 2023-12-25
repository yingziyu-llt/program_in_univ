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

struct Node
{
	int val,now,pre;
}a[10000];
int num[10000];
int cnt[10000];
int main()
{
#ifdef llt
	freopen("5.in","r",stdin);
	freopen("5.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	int tot = 0;
	for(int i = 1;i <= n;i++)
		scanf("%d",&num[i]),tot += num[i],cnt[num[i]]++;
	int c1,c2;
	scanf("%d%d",&c1,&c2);
	for(int i = 1;i <= c1;i++)
		a[i].val = 0;
	for(int i = 1;i <= n;i++)
		for(int j = c1;j >= num[i];j--)
			if(a[j].val < a[j - num[i]].val + num[i])
			{
				a[j].val = a[j - num[i]].val + num[i];
				a[j].pre = j - num[i];
				a[j].now = num[i];
			}
	int ccnt = 0,ans[100];
	if(tot - a[c1].val > c2)
	{
		puts("can't find a way to Loading");
		return 0;
	}
	puts("ok,can load it");
	puts("a way is:");
	printf("the first trip load:");
	for(int i = c1;i != 0;i = a[i].pre)
	{
		ans[++ccnt] = a[i].now;
		cnt[a[i].now]--;
		if(ans[ccnt] == a[i].val) break;
	}
	printf("%d",ans[ccnt]);
	for(int i = ccnt - 1;i > 0;i--)
		printf(" %d",ans[i]);
	puts("");
	printf("the second trip load:");
	int tt = 0;
	for(int i = 1;i <= n;i++)
	{
		if(cnt[num[i]] == 0) continue;
		cnt[num[i]]--;
		if(tt == 0)
			printf("%d",num[i]);
		else
			printf(" %d",num[i]);
		tt++;
	}
}
