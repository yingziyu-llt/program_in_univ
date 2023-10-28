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
#include<string>
using namespace std;

const int md[2][13] = {{0,31,28,31,30,31,20,31,31,30,31,30,31},{0,31,29,31,30,31,20,31,31,30,31,30,31}};

bool is_run(int i)
{
	if(i % 400 == 0) return true;
	if(i % 100 != 0 && i % 4 == 0)
		return true;
	return false;
}

char s[100010],cnt;

int main()
{
#ifdef llt
	freopen("8_2.in","r",stdin);
	freopen("8_2.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	for(int y = 2021;y <= 2030;y++)
	{
		int run = is_run(y);
		for(int m = 1;m <= 12;m++)
			for(int d = 1;d <= md[run][m];d++)
			{
				if(y == 2021 && m == 1 && d == 1)
					d = 3;
				sprintf(s + cnt,"%d%2d%2d",y,m,d);
				cnt += 8;
			}
	}
	for(int i = 1;i <= n;i++)
	{
		int a;
		scanf("%d",&a);
		if(a < 10)
		{
			int len=strlen(s);
			for(int j = 0;j < len;j++)
				if(s[j] == a + '0')
				{
					printf("%d\n",j + 1);
					break;
				}
		}
		else
		{
			int len = strlen(s);
			char ch1 = a / 10 + '0',ch2 = a % 10 + '0';
			for(int j = 0;j < len - 1;j++)
			{
				if(s[j] == ch1 && s[j + 1] == ch2)
				{
					printf("%d\n",j + 1);
					break;
				}
			}
		}
		fflush(stdout);
	}
}

