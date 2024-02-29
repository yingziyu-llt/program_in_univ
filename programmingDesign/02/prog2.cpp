#include <cstdlib>
#include <cstring>
#include <iostream>
using namespace std;
class Complex {
private:
	double r, i;

public:
	void Print()
	{
		cout << r << "+" << i << "i" << endl;
	}

	int toNumber(char* s)
	{
		int ans = 0;
		for (int i = 0; i < strlen(s); i++)
			ans = ans * 10 + s[i] - '0';
		return ans;
	}
	Complex(char* s)
	{
		char ss[20];
		strcpy(ss, s);
		char* p = strtok(ss, "+");
		r = toNumber(p);
		p = strtok(NULL, "i");
		i = toNumber(p);
	}
	Complex()
	{
		r = 0, i = 0;
	}

	// 在此处补充你的代码
};
int main()
{
	Complex a;
	a = "3+4i";
	a.Print();
	a = "5+6i";
	a.Print();
	return 0;
}
