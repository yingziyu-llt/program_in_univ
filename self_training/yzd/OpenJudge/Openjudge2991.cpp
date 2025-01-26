#include <iostream>
#include <string>
using namespace std;

int ksm(int base, string exp, int mod) {
    int result = 1;
    base %= mod;  

    while (exp.length() > 0) {
        if ((exp.back() - '0') % 2 == 1) {
            result = (result * base) % mod;
        }

        int carry = 0;
        for (int i = 0; i < exp.length(); i++) {
            int digit = (exp[i] - '0') + carry * 10;
            exp[i] = (digit / 2) + '0';
            carry = digit % 2;
        }

        while (exp.length() > 0 && exp[0] == '0') {
            exp.erase(exp.begin());
        }

        base = (base * base) % mod;
    }

    return result;
}

int main() {
    int k;
    cin >> k;
    while (k--) {
        string n;
        cin >> n;
        int result = ksm(2011, n, 10000);
        cout << result << endl;
    }
    return 0;
}