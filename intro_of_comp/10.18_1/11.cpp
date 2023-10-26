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

stack <int> st;
char ans[100010];
int main()
{
#ifdef llt
	freopen("11.in","r",stdin);
	freopen("11.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	while(n--)
	{
		while(!st.empty())
			st.pop();
		char s[110];
		scanf("%s",s);
		memset(ans,0,sizeof(ans));
		for(int i = 0;i < (int)strlen(s);i++)
		{
			if(s[i] == '(')
				st.push(i),ans[i] = ' ';
			else if(s[i] == ')')
			{
				if(!st.empty())
					st.pop(),ans[i] = ' ';
				else
					ans[i] = '?'; 
			}
			else
				ans[i] = ' ';
		}
		while(!st.empty())
			ans[st.top()] = '$',st.pop();
		puts(s);
		puts(ans);
	}
}

