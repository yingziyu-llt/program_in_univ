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

void split(char *s,int *intn,int *floatn)
{
	int a = 0;
	for(a = 0;a < (int)strlen(s);a++)
		if(s[a] == '.')
			break;
	for(int i = a - 1;i >= 0;i--)
		intn[a - i - 1] = s[i] - '0';
	for(int i = a + 1;i < (int)strlen(s);i++)
		floatn[i - a - 1] = s[i] - '0';
}
int main()
{
#ifdef llt
	freopen("8.in","r",stdin);
	freopen("8.out","w",stdout);
#endif
	int t;
	scanf("%d",&t);
	while(t--)
	{
		char str1[110],str2[110];
		scanf("%s\n%s",str1,str2);
		int num1_int[100],num1_float[100];
		int num2_int[100],num2_float[100];
		memset(num1_int,0,sizeof(num1_int));
		memset(num1_float,0,sizeof(num1_float));
		memset(num2_int,0,sizeof(num2_int));
		memset(num2_float,0,sizeof(num2_float));
		split(str1,num1_int,num1_float);
		split(str2,num2_int,num2_float);
		for(int i = 99;i >= 1;i--)
		{
			num1_float[i] += num2_float[i];
			num1_float[i - 1] += num1_float[i] / 10;
			num1_float[i] %= 10;
		}
		num1_float[0] += num2_float[0];
		num1_int[0] += num1_float[0] / 10;
		num1_float[0] %= 10;
		for(int i = 0;i < 99;i++)
		{
			num1_int[i] += num2_int[i];
			num1_int[i + 1] += num1_int[i] / 10;
			num1_int[i] %= 10;
		}

		int from1 = 99;
		while(num1_int[from1] == 0 && from1)
			from1--;
		for(int i = from1;i >= 0;i--)
			printf("%d",num1_int[i]);
		from1 = 99;
		putchar('.');
		while(num1_float[from1] == 0 && from1)
			from1--;
		for(int i = 0;i <= from1;i++)
			printf("%d",num1_float[i]);
		puts("");
	}
}


