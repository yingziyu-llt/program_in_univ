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

bool is_B_alpha(char c)
{
	return (c >= 'A' && c <= 'Z');
}
bool is_S_alpha(char c)
{
	return (c >= 'a' && c <= 'z');
}
int main()
{
#ifdef llt
	freopen("5.in","r",stdin);
	freopen("5.out","w",stdout);
#endif
	int a,b;
	char s[100];
	scanf("%d %s %d",&a,s,&b);
	int n = strlen(s);
	int demincal = 0;
	for(int i = 0;i < n;i++)
	{
		if(is_B_alpha(s[i]))
			demincal = demincal * a + s[i] - 'A' + 10;
		else if(is_S_alpha(s[i]))
			demincal = demincal * a + s[i] - 'a' + 10;
		else
			demincal = demincal * a + s[i] - '0';
	}
	n = 0;
	while(demincal)
	{
		int ret = demincal % b;
		s[++n] = ret > 9 ? ret - 10 + 'A' : ret + '0';
		demincal /= b;
	}
	for(int i = n;i > 0;i--)
		printf("%c",s[i]);

}
