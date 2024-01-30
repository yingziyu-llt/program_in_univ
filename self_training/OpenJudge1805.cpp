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
char s[1000];
int anslab[1000],curlab[1000];
int sum;
int ans = -1,res = 0,cnt = 0,len;
void dfs(int pos,int cur)
{
    if(pos == len + 1)
    {
        res += cur;
        if(res <= sum)
        {
            if(res == ans) cnt++;
            else if(res > ans)
			{
				ans = res,cnt = 1;
				for(int i = 1;i <= len;i++)
					anslab[i] = curlab[i];
			}
        }
        res -= cur;
        return ;
    }
    if(res > sum) return ;
    res += cur;
	curlab[pos] = 1;
    dfs(pos + 1,s[pos] - '0');
	curlab[pos] = 0;
    res -= cur;
    if(pos != 1)
        dfs(pos + 1,cur * 10 + s[pos] - '0');
}

int main()
{
#ifdef llt
	freopen("OpenJudge1805.in","r",stdin);
	freopen("OpenJudge1805.out","w",stdout);
#endif
    while(1)
    {
        scanf("%d",&sum);
		memset(s,0,sizeof(s));
		ans = -1;
		res = 0;
        scanf("%s",s + 1);
        if(s[1] == '0' && sum == 0)
            return 0;
        len = strlen(s + 1);
        dfs(1,0);
        if(ans == -1)
            printf("error\n");
        else if(cnt > 1)
            printf("rejected\n");
        else
		{
            printf("%d ",ans);
			for(int i = 1;i < len;i++)
			{
				if(anslab[i + 1])
					printf("%c ",s[i]);
				else
					printf("%c",s[i]);
			}
			printf("%c\n",s[len]);
		}
    }

}
