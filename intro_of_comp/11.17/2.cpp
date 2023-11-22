#include<stdio.h>
#include<math.h>
#include<string.h>
using namespace std;

// C++初始模板程序

char s[1000];
int cnt[100];

int main() {
#ifdef llt
	freopen("2.in","r",stdin);
	freopen("2.out","w",stdout);
#endif
    scanf("%s",s);
    int len = strlen(s);
    for(int i = 0;i < len;i++)
    {
        if((s[i] >= 'a' && s[i] <= 'z'))
            cnt[s[i] - 'a']++;
        if((s[i] >= 'A' && s[i] <= 'Z'))
            cnt[s[i] - 'A']++;
    }
    int max1 = 0,max2 = 0;
    for(int i = 0;i <= 25;i++)
		max1 = max(max1,cnt[i]);
	for(int i = 0;i <= 25;i++)
		if(cnt[i] == max1)
			cnt[i] = 0;
	for(int i = 0;i <= 25;i++)
		max2 = max(max2,cnt[i]);

    for(int i = 0;i < len;i++)
    {
        if((s[i] >= 'a' && s[i] <= 'z'))
            if(cnt[s[i] - 'a'] == max2)
            {
                printf("%c+%c:%d",s[i] - 'a' + 'A',s[i],max2);
                return 0;
            }
        if((s[i] >= 'A' && s[i] <= 'Z'))
            if(cnt[s[i] - 'A'] == max2)
            {
                printf("%c+%c:%d",s[i],s[i] - 'A' + 'a',max2);
                return 0;
            }
    }
	return 0;
}

