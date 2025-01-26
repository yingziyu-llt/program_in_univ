#include<algorithm>
#include<iostream>

using namespace std;

struct Node {
    int x, y;
} a[100005];

bool cmp(Node a,Node b) { 
    if(a.x == b.x) return a.y > b.y;
    return a.x < b.x;
}

int main() {
    int n;
    cin >> n;
    for(int i = 1;i <= n;i++) {
        cin >> a[i].x >> a[i].y;
    }
    sort(a + 1,a + n + 1,cmp);
    int max_l = a[1].y;
    for(int i = 2;i <= n;i++) {
        if(a[i].x > max_l) {
            cout << "no" << endl;
            return 0;
        }
        max_l = max(max_l,a[i].y);
    }
    cout << a[1].x << " " << max_l << endl;
}
