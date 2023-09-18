#include<stdio.h>
#include<algorithm>
#include<string.h>
#include<math.h>
using namespace std;
int dp[1 << 20];
int main()
{
#ifdef llt
	freopen("LuoguP8687.in","r",stdin);
	freopen("LuoguP8687.out","w",stdout);
#endif
	int n,m,k;
	scanf("%d%d%d",&n,&m,&k);
	memset(dp,0x7f,sizeof(dp));
	dp[0] = 0;
	for(int i = 1;i <= n;i++)
	{
		int it = 0;
		for(int j = 1;j <= k;j++)
		{
			int p;
			scanf("%d",&p);
			it = it | (1 << (p - 1));
		}
		for(int j = 0;j <= (1 << m) - 1;j++)
			dp[j | it] = min(dp[j] + 1,dp[j | it]);
	}
	printf("%d",dp[(1 << m) - 1] > n ? -1 : dp[(1 << m) - 1]);
	return 0;
}

