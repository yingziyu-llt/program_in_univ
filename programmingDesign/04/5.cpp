#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iterator>
using namespace std;
const int MAX = 110;
class CHugeInt {
public:
	int data[1000];
	int length;
	CHugeInt(int n)
	{
		length = 0;
		memset(data, 0, sizeof(data));
		while (n) {
			data[length++] = n % 10;
			n /= 10;
		}
	}
	CHugeInt(char const* s)
	{
		memset(data, 0, sizeof(data));
		length = strlen(s);
		for (int i = 0; i < length; i++)
			data[i] = s[length - i - 1] - '0';
	}
	CHugeInt& operator=(CHugeInt const& xx)
	{
		length = xx.length;
		memcpy(data, xx.data, sizeof(data));
		return *this;
	}
	CHugeInt operator+(int n)
	{
		CHugeInt ans(0);
		CHugeInt b(n);
		ans = *this + b;
		return ans;
	}
	CHugeInt operator+(CHugeInt x)
	{
		CHugeInt ans(0);
		int len = max(this->length, x.length) + 2;
		for (int i = 0; i < len; i++)
			ans.data[i] += data[i] + x.data[i], ans.data[i + 1] += ans.data[i] / 10, ans.data[i] %= 10;
		for (int i = len; i >= 0; i--)
			if (ans.data[i] != 0) {
				ans.length = i + 1;
				break;
			}
		return ans;
	}
	operator char*()
	{
		char* s;
		if (length == 0) {
			s = new char[2];
			s[0] = '0';
			s[1] = 0;
			return s;
		}
		s = new char[length + 1];
		for (int i = 0; i < length; i++)
			s[i] = data[length - i - 1] + '0';
		s[length] = 0;
		return s;
	}
	friend CHugeInt operator+(int n, CHugeInt& x)
	{
		return x + n;
	}
	CHugeInt& operator+=(int n)
	{
		CHugeInt x(n);
		*this = *this + x;
		return *this;
	}
	CHugeInt& operator++()
	{
		*this += 1;
		return *this;
	}
	CHugeInt operator++(int x)
	{
		CHugeInt res = *this;
		++*this;
		return res;
	}
	// 在此处补充你的代码
};
int main()
{
	char s[210];
	int n;

	while (cin >> s >> n) {
		CHugeInt a(s);
		CHugeInt b(n);

		cout << a + b << endl;
		cout << n + a << endl;
		cout << a + n << endl;
		b += n;
		cout << ++b << endl;
		cout << b++ << endl;
		cout << b << endl;
	}
	return 0;
}
