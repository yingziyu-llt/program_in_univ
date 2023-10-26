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

double a[10010],b[10010];

void my_sort(double *ss,int a,int b)
{
	for(int i = 1;i <= b;i++)
		for(int j = a;j < b;j++)
			if(ss[j] > ss[j + 1])
				swap(ss[j],ss[j + 1]);
}
int main()
{
#ifdef llt
	freopen("1.in","r",stdin);
	freopen("1.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	int len1 = 0,len2 = 0;
	for(int i = 1;i <= n;i++)
	{
		char s[10];
		memset(s,0,sizeof(s));
		scanf("%s",s);
		if(strcmp(s,"male") == 0)
			scanf("%lf",&b[++len2]);
		else
			scanf("%lf",&a[++len1]),a[len1] *= -1;
	}
	my_sort(a,1,len1);
	my_sort(b,1,len2);
	for(int i = 1;i <= len2;i++)
		printf("%.2lf ",b[i]);

	for(int i = 1;i < len1;i++)
		printf("%.2lf ",-a[i]);
	printf("%.2lf",-a[len1]);
}

