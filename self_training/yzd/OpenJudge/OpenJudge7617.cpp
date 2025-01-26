#include <iostream>
using namespace std;
int a[110000];
void quick_sort(int l,int r){
    if(l >= r)
        return;
    int i = l,j = r,x = a[l];
    while(i < j) {
        while(i < j && a[j] <= x)
            j--;
        while(i < j && a[i] >= x)
            i++;
        if(i < j) {
            int t = a[i];
            a[i] = a[j];
            a[j] = t;
        }
    }
    a[l] = a[i];
    a[i] = x;
    quick_sort(l,i - 1);
    quick_sort(i + 1,r);
}
int main(){
    int n;
    cin >> n;
    for(int i = 1;i <= n;i++)
        cin >> a[i];
    int k;
    cin >> k;
    quick_sort(1,n);
    for(int i = 1;i <= k;i++)
        cout << a[i] << endl;
}