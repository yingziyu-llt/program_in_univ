#include <cctype>
#include <cmath>
#include <iostream>
#include <sstream>
#include <stack>
#include <string>

using namespace std;

int precedence(char op)
{
	if (op == '^')
		return 3;
	else if (op == '*' || op == '/')
		return 2;
	else if (op == '+' || op == '-')
		return 1;
	else
		return -1;
}

bool isOperator(char ch)
{
	return (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '^');
}

bool isDigit(char* ch)
{
	return isdigit(*ch) || (*ch == '-' && isdigit(*(ch + 1)));
}

bool isLeftParenthesis(char ch)
{
	return (ch == '(');
}

bool isRightParenthesis(char ch)
{
	return (ch == ')');
}

string infixToPostfix(const string& infix)
{
	stringstream output;
	stack<char> s;

	istringstream iss(infix);
	string token;
	while (iss >> token) {
		if (isDigit(&token[0]))
			output << token << " ";
		else if (isOperator(token[0])) {
			while (!s.empty() && precedence(s.top()) >= precedence(token[0]) && !isLeftParenthesis(s.top())) {
				output << s.top() << " ";
				s.pop();
			}
			s.push(token[0]);
		} else if (isLeftParenthesis(token[0]))
			s.push(token[0]);
		else if (isRightParenthesis(token[0])) {
			while (!s.empty() && !isLeftParenthesis(s.top())) {
				output << s.top() << " ";
				s.pop();
			}
			if (!s.empty())
				s.pop(); // Discard the left parenthesis
		}
	}

	while (!s.empty()) {
		output << s.top() << " ";
		s.pop();
	}

	return output.str();
}

int evaluatePostfix(const string& postfix)
{
	stack<int> s;
	stringstream ss(postfix);
	string token;

	while (ss >> token) {
		if (isdigit(token[0]) || (token.size() > 1 && token[0] == '-' && isdigit(token[1]))) {
			s.push(stoi(token));
		} else if (isOperator(token[0])) {
			int operand2 = s.top();
			s.pop();
			int operand1 = s.top();
			s.pop();
			switch (token[0]) {
			case '+':
				s.push(operand1 + operand2);
				break;
			case '-':
				s.push(operand1 - operand2);
				break;
			case '*':
				s.push(operand1 * operand2);
				break;
			case '/':
				s.push(operand1 / operand2);
				break;
			case '^':
				s.push(pow(operand1, operand2));
				break;
			}
		}
	}

	return s.top();
}

int main()
{
	int N;
	cin >> N;
	cin.ignore(); // Ignore newline character

	for (int i = 0; i < N; ++i) {
		string infix;
		getline(cin, infix);
		string postfix = infixToPostfix(infix);
		int result = evaluatePostfix(postfix);
		cout << result << endl;
	}

	return 0;
}
