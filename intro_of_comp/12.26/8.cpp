#include <iostream>
#include <stack>
#include <sstream>

using namespace std;

bool isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/';
}

int getPriority(char op) {
    if (op == '+' || op == '-') {
        return 1;
    } else if (op == '*' || op == '/') {
        return 2;
    } else {
        return 0; // 非法操作符
    }
}

int operate(int operand1, char op, int operand2) {
    switch (op) {
        case '+':
            return operand1 + operand2;
        case '-':
            return operand1 - operand2;
        case '*':
            return operand1 * operand2;
        case '/':
            return operand1 / operand2;
        default:
            return 0; // 非法操作符
    }
}

int calculate(string expression) {
    stack<int> operandStack;
    stack<char> operatorStack;

    for (size_t i = 0; i < expression.size(); ++i) {
        char c = expression[i];

        if (isdigit(c) || (c == '-' && (i == 0 || expression[i - 1] == '('))|| (c == '+' && (i == 0 || expression[i - 1] == '('))) {
            // 处理数字，包括负号作为负数的标志
            stringstream ss;
            ss << c;
            int num;
            ss >> num;
            operandStack.push(num);
        } else if (isOperator(c)) {
            while (!operatorStack.empty() && getPriority(operatorStack.top()) >= getPriority(c)) {
                int operand2 = operandStack.top();
                operandStack.pop();
                int operand1 = operandStack.top();
                operandStack.pop();
                char op = operatorStack.top();
                operatorStack.pop();
                int result = operate(operand1, op, operand2);
                operandStack.push(result);
            }
            operatorStack.push(c);
        } else if (c == '(') {
            operatorStack.push(c);
        } else if (c == ')') {
            while (!operatorStack.empty() && operatorStack.top() != '(') {
                int operand2 = operandStack.top();
                operandStack.pop();
                int operand1 = operandStack.top();
                operandStack.pop();
                char op = operatorStack.top();
                operatorStack.pop();
                int result = operate(operand1, op, operand2);
                operandStack.push(result);
            }
            operatorStack.pop();
        }
    }

    while (!operatorStack.empty()) {
        int operand2 = operandStack.top();
        operandStack.pop();
        int operand1 = operandStack.top();
        operandStack.pop();
        char op = operatorStack.top();
        operatorStack.pop();
        int result = operate(operand1, op, operand2);
        operandStack.push(result);
    }

    return operandStack.top();
}

int main() {
    string expression;
    while (getline(cin, expression)) {
        if (!expression.empty()) {
            int result = calculate(expression);
            cout << result << endl;
        }
        if (cin.eof()) {
            break;  // 如果输入结束，结束循环
        }
    }

    return 0;
}

