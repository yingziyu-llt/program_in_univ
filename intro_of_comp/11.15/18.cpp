#include <iostream>
#include <algorithm>
using namespace std;

const int MAXN = 16;
const int INF = 1e9;
int n;
int dp[1<<MAXN][MAXN], cost[MAXN][MAXN];
 
int tsp(int mask, int pos) {
    if(mask==(1<<n)-1)
        return cost[pos][0];
    if(dp[mask][pos]!=-1)
        return dp[mask][pos];
 
    int ans = INF;
    for(int i=0; i<n; i++) {
        if((mask&(1<<i))==0)
            ans = min(ans, cost[pos][i] + tsp(mask|(1<<i), i));
    }
    return dp[mask][pos] = ans;
}
 
int main() {
    cin>>n;
    for(int i=0; i<n; i++) {
        for(int j=0; j<n; j++) {
            cin>>cost[i][j];
        }
    }
    for(int i=0; i<(1<<n); i++) {
        for(int j=0; j<n; j++) {
            dp[i][j] = -1;
        }
    }
    cout<<tsp(1, 0)<<endl;  // start from city 0
    return 0;
}
