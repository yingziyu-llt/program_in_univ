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
#include<stack>
using namespace std;

int a[1000100],b[1000100],c[1000100];
stack <int> s;
int main()
{
#ifdef llt
	freopen("LuoguP6155.in","r",stdin);
	freopen("LuoguP6155.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	for(int i = 1;i <= n;i++)
		scanf("%d",&a[i]);
	for(int i = 1;i <= n;i++)
		scanf("%d",&b[i]);
	sort(a + 1,a + 1 + n);sort(b + 1,b + 1 + n);
	int cur = 1,cnt = 1;
	for(int i = 1;i <= a[n] + n;i++)
	{
		if(a[cur] > i && s.empty())
		{
			i = a[cur] - 1;
			continue;
		}
		while(a[cur] == i)
			s.push(a[cur]),cur++;
		if(s.empty()) continue;
		c[cnt] = i - s.top();
		cnt++;
		s.pop();
	}
	sort(c + 1,c + n + 1);
	unsigned long long ans = 0;
	for(int i = 1;i <= n;i++)
		ans += c[i] * b[n - i + 1];
	printf("%ulld",ans);
	return 0;
}

