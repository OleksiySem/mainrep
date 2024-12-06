#include <iostream>
#include <string>
#include <cctype>
#include <sstream>
#include <stdexcept>

struct CharStack {
    std::string data[100];
    int top = -1;

    void push(const std::string& c) {
        if (top >= 99) {
            throw std::overflow_error("Stack overflow");
        }
        data[++top] = c;
    }

    std::string pop() {
        if (top < 0) {
            throw std::underflow_error("Stack underflow");
        }
        return data[top--];
    }

    std::string peek() const {
        if (top < 0) return "";
        return data[top];
    }

    bool isEmpty() const {
        return top == -1;
    }
};

struct NumberStack {
    double data[100];
    int top = -1;

    void push(double value) {
        if (top >= 99) {
            throw std::overflow_error("Stack overflow");
        }
        data[++top] = value;
    }

    double pop() {
        if (top < 0) {
            throw std::underflow_error("Stack underflow");
        }
        return data[top--];
    }

    bool isEmpty() const {
        return top == -1;
    }
};

bool isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '!';
}

int precedence(char op) {
    if (op == '!') return 3;
    if (op == '*' || op == '/') return 2;
    if (op == '+' || op == '-') return 1;
    return 0;
}

double factorial(int n) {
    if (n < 0) throw std::runtime_error("Factorial of a negative number is undefined");
    double result = 1;
    for (int i = 1; i <= n; ++i) {
        result *= i;
    }
    return result;
}

std::string toRPN(const std::string& expression) {
    CharStack operators;
    std::string output;
    size_t i = 0;

    while (i < expression.length()) {
        if (std::isspace(expression[i])) {
            i++;
            continue;
        }

        if (std::isdigit(expression[i]) || (expression[i] == '-' && (i == 0 || isOperator(expression[i - 1]) || expression[i - 1] == '('))) {
            std::string number;
            if (expression[i] == '-') {
                number += expression[i++];
            }
            while (i < expression.length() && (std::isdigit(expression[i]) || expression[i] == '.')) {
                number += expression[i++];
            }
            output += number + ' ';
        }
        else if (expression[i] == '(') {
            operators.push(std::string(1, expression[i]));
            i++;
        }
        else if (expression[i] == ')') {
            while (!operators.isEmpty() && operators.peek() != "(") {
                output += operators.pop() + ' ';
            }
            if (!operators.isEmpty()) operators.pop();
            i++;
        }
        else if (isOperator(expression[i])) {
            while (!operators.isEmpty() && precedence(operators.peek()[0]) >= precedence(expression[i])) {
                output += operators.pop() + ' ';
            }
            operators.push(std::string(1, expression[i]));
            i++;
        }
        else {
            throw std::runtime_error("Invalid character in expression");
        }
    }

    while (!operators.isEmpty()) {
        output += operators.pop() + ' ';
    }

    return output;
}

double evaluateRPN(const std::string& rpn) {
    NumberStack stack;
    std::istringstream stream(rpn);
    std::string token;


    while (stream >> token) {
        if (std::isdigit(token[0]) || (token[0] == '-' && token.length() > 1)) {
            stack.push(std::stod(token));
        }
        else if (token == "!") {
            if (stack.isEmpty()) throw std::runtime_error("Invalid RPN: missing operand for factorial");
            double operand = stack.pop();
            stack.push(factorial(static_cast<int>(operand)));
        }
        else if (isOperator(token[0]) && token.length() == 1) {
            if (stack.isEmpty()) throw std::runtime_error("Invalid RPN: missing operands");
            double b = stack.pop();
            if (stack.isEmpty()) throw std::runtime_error("Invalid RPN: missing operands");
            double a = stack.pop();

            switch (token[0]) {
            case '+': stack.push(a + b); break;
            case '-': stack.push(a - b); break;
            case '*': stack.push(a * b); break;
            case '/':
                if (b == 0) throw std::runtime_error("Division by zero");
                stack.push(a / b);
                break;
            default: throw std::runtime_error("Unknown operator");
            }
        }
        else {
            throw std::runtime_error("Invalid token in RPN");
        }
    }

    if (stack.isEmpty()) throw std::runtime_error("Invalid RPN: no result");
    double result = stack.pop();
    if (!stack.isEmpty()) throw std::runtime_error("Invalid RPN: extra operands");
    return result;
}

int main() {
    std::string expression;

    std::cout << "Enter a mathematical expression: ";
    std::getline(std::cin, expression);

    try {
        std::string rpn = toRPN(expression);
        std::cout << "RPN: " << rpn << std::endl;

        double result = evaluateRPN(rpn);
        std::cout << "Result: " << result << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
