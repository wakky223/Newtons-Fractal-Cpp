#pragma once
#include "complex.hpp"

#ifdef __DEBUG
#define assert(expr) if(!expr) std::cout << "Assert error on line " << __LINE__ << " in file " << __FILE__ << std::endl; throw __LINE__
#else 
#define assert(expr)
#endif


enum {
    NUMBER = 0, 
    VARIABLE = 1,
    OPENP = 2,
    CLOSEP = 3,
    ADD = 4,
    SUBTRACT = 5,
    MULTIPLY = 6,
    DIVIDE = 7,
    POWER = 8,
    SIN = 9,
    COS = 10,
    TAN = 11,
    SEC = 12,
    CSC = 13,
    COT = 14
};

class func
{
public:
    func() {

    }

    /// @brief init function object
    /// @param funcString function string
    void init(std::string funcString) {
        function_string = funcString;

        //remove all spaces
        for (int i = 0; i < function_string.length(); i++) {
            if (function_string[i] == ' ') {
                function_string.erase(i, 1);
                i--;
            }
        }

        try {
            CheckBrackets(funcString);
            tokenize(funcString);
            pre_format();
            parse_tokens(tokens);
            while(simplify());
            std::cout << "Interpreted as: " << get_tokens() << std::endl;
        }
        catch (int exc) {
            std::cout << "Error parsing function. ";
            switch (exc) {
            case 0:
                std::cout << "Cannot have \"()\"" << std::endl;
                break;
            case 1:
                std::cout << "Cannot have \"( operator\"" << std::endl;
                break;
            case 2:
                std::cout << "Cannot have two adjacent operators" << std::endl;
                break;
            case 4:
                std::cout << "This is most likely an unknown charecter" << std::endl;
                break;
            case 5:
                std::cout << "Can't do powers" << std::endl;
                break;
            default:
                std::cout << "Assert error on line number " << exc << "in file " << __FILE__ <<std::endl;
            }
        }
    }


    /// @brief init function object, repeatedly ask the user for a function until a valid function is provided
    void init() {
        while (true) {
            tokens.clear();
            std::cout << "Enter a function: ";
            std::cin >> function_string;
            if (std::cin.fail()){
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }

            //remove all spaces
            for (int i = 0; i < function_string.length(); i++) {
                if (function_string[i] == ' ') {
                    function_string.erase(i, 1);
                    i--;
                }
            }

            try {
                CheckBrackets(function_string);
                tokenize(function_string);
                pre_format();
                parse_tokens(tokens);
                while(simplify());
                std::cout << "Interpreted as: " << get_tokens() << std::endl;
                break;
            }
            catch (int exc) {
                switch (exc) {
                case 0:
                    std::cout << "Cannot have \"()\"" << std::endl;
                    break;
                case 1:
                    std::cout << "Cannot have \"( operator\"" << std::endl;
                    break;
                case 2:
                    std::cout << "Cannot have two adjacent operators" << std::endl;
                    break;
                case 4:
                    std::cout << "Error when trying to parse function, this is most likely an unknown character" << std::endl;
                    break;
                case 5:
                    std::cout << "Can't do powers yet lol" << std::endl;
                    break;
                default:
                    std::cout << "Assert error on line number " << exc << std::endl;
                }
            }
        }
    }

    std::string function_string = "";
    std::vector<std::string> tokens;
    std::vector<complex> number_stack;
    std::vector<unsigned short> stack;

    /// @return String that represents the function after being converted to RPN
    std::string RPN() {
        std::string output = "";
        for (int i = 0; i < stack.size(); i++) {
            switch (stack[i])
            {
            case NUMBER:
                output += string(number_stack[i]) + " ";
                break;
            case VARIABLE:
                output += "x ";
                break;
            case POWER:
                output += "^ ";
                break;
            case MULTIPLY:
                output += "* ";
                break;
            case DIVIDE:
                output += "/ ";
                break;
            case ADD:
                output += "+ ";
                break;
            case SUBTRACT:
                output += "- ";
                break;
            case SIN:
                output += "sin ";
                break;
            case COS:
                output += "cos ";
                break;
            case TAN:
                output += "tan ";
                break;
            case SEC:
                output += "sec ";
                break;
            case CSC:
                output += "csc ";
                break;
            case COT:
                output += "cot ";
                break;
            default:
                break;
            }
        }
        return output;
    }

    /// @return function in infix notation
    std::string get_tokens() {
        std::string output = "";
        for (int i = 0; i < tokens.size(); i++) {
            output += tokens[i] + " ";
        }
        return output;
    }

    std::string get_tokens(std::vector<std::string> input) {
        std::string output = "";
        for (int i = 0; i < input.size(); i++) {
            output += input[i] + " ";
        }
        return output;
    }

    /// @brief evaluates the function
    complex evaluate_function(complex input) {
        return evaluateRPN(stack.size() - 1, input);
    }

private:
    /// @brief Figures out the type of what the first thing is in a string
    /// @param input input string
    short type(std::string input) {
        switch (input[0]) {
        case '(':
            return OPENP;
        case ')':
            return CLOSEP;
        case '*':
            return MULTIPLY;
        case '+':
            return ADD;
        case '-':
            return SUBTRACT;
        case '/':
            return DIVIDE;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case 'i':
            return NUMBER;
        case '^':
            return POWER;
        case 'x':
            return VARIABLE;
        }
        if (input == "sin") {
            return SIN;
        }
        else if (input == "cos") {
            return COS;
        }
        else if (input == "tan") {
            return TAN;
        }
        else if (input == "sec") {
            return SEC;
        }
        else if (input == "csc") {
            return CSC;
        }
        else if (input == "cot") {
            return COT;
        }
        throw(4);
    }

    /// @brief Change a string into tokens in the form of two vectors
    void tokenize(std::string input) {
        while (input.length() != 0) {
            //finds what type the token is, and 
            if (input[0] == '.') {
                //the first charecter is a decimal point:
                std::string newtoken = "0.";
                for (int i = 1; input[i] >= '0' && input[i] <= '9'; i++) {
                    newtoken.push_back(input[i]);
                }
                tokens.push_back(newtoken);
                input.erase(0, newtoken.length() - 1);
            }
            else if ((input[0] >= '0' && input[0] <= '9')) {
                //the first charecter is a number:
                std::string newtoken = "";
                for (int i = 0; (input[i] >= '0' && input[i] <= '9') || input[i] == '.'; i++) {
                    newtoken.push_back(input[i]);
                }
                tokens.push_back(newtoken);
                input.erase(0, newtoken.length());
            }
            else if (input[0] == '(') {
                //a open parentheses
                tokens.push_back("(");
                input.erase(0, 1);
            }
            else if (input[0] == ')') {
                //a open parentheses
                tokens.push_back(")");
                input.erase(0, 1);
            }
            else if (input[0] == 'x') {
                //the variable
                tokens.push_back("x");
                input.erase(0, 1);
            }
            else if (input[0] == '+') {
                //plus operator
                tokens.push_back("+");
                input.erase(0, 1);
            }
            else if (input[0] == '-') {
                //minus operator
                tokens.push_back("-");
                input.erase(0, 1);
            }
            else if (input[0] == '*') {
                //multiply operator
                tokens.push_back("*");
                input.erase(0, 1);
            }
            else if (input[0] == '/') {
                //divide operator
                tokens.push_back("/");
                input.erase(0, 1);
            }
            else if (input[0] == '^') {
                //power operator
                throw 5;
                tokens.push_back("^");
                input.erase(0, 1);
            }
            else if (input[0] == 'e') {
                //e
                tokens.push_back("2.718281828");
                input.erase(0, 1);
            }
            else if (input[0] == 'p' && input[1] == 'i') {
                //pi
                tokens.push_back("3.1415926535");
                input.erase(0, 2);
            }
            else if (input[0] == 'i') {
                //imaginary constant
                tokens.push_back("i");
                input.erase(0, 1);
            }
            else {
                //if none of the other options are true the string must start with a function.
                for (int i = 1; i <= input.length(); i++) {
                    if ((input[i] < 'a' || input[i] > 'z') || input[i] == 'x') {
                        tokens.push_back(input.substr(0, i));
                        input.erase(0, i);
                        break;
                    }
                    else if (i == input.length()) {
                        throw 4;
                    }
                }
            }
        }
    }

    /// @brief Throw an error if the function has invalid syntax. add multiplication signs where it would be implied
    void pre_format() {
        if (tokens.size() < 2) {
            throw 4;
        }
        for (int i = 0; i < tokens.size() - 1; i++) {
            switch (type(tokens[i]))
            {
            case OPENP:
                switch (type(tokens[i + 1]))
                {
                case CLOSEP:
                    //we have "(" , ")"
                    throw 0;
                    break;
                case POWER:
                case MULTIPLY:
                case DIVIDE:
                case ADD:
                case SUBTRACT:
                    //we have "(" , operator
                    if (tokens[i + 1] == "-") {
                        tokens.insert(tokens.begin() + i + 1, "0");
                        break;
                    }
                    throw 1;
                    break;
                }
                break;
            case CLOSEP:
                switch (type(tokens[i + 1]))
                {
                case OPENP:
                    //we have ")" , "("
                    tokens.insert(tokens.begin() + i + 1, "*");
                    break;
                case NUMBER:
                    //we have ")" , number
                    tokens.insert(tokens.begin() + i + 1, "*");
                    break;
                case VARIABLE:
                    //we have ")" , x
                    tokens.insert(tokens.begin() + i + 1, "*");
                    break;
                case POWER:
                case MULTIPLY:
                case DIVIDE:
                case ADD:
                case SUBTRACT:
                case CLOSEP:
                    break;
                default:
                    //we have ")" , function
                    tokens.insert(tokens.begin() + i + 1, "*");
                    break;
                }
                break;

            case POWER:
            case MULTIPLY:
            case DIVIDE:
            case ADD:
            case SUBTRACT:
                switch (type(tokens[i + 1]))
                {
                case POWER:
                case MULTIPLY:
                case DIVIDE:
                case ADD:
                case SUBTRACT:
                    //we have operator , operator
                    if (tokens[i + 1] == "-") {
                        break;
                    }
                    throw 2;
                    break;
                }
                break;
            case NUMBER:
                switch (type(tokens[i + 1]))
                {
                case OPENP:
                    //we have number , "("
                    tokens.insert(tokens.begin() + i + 1, "*");
                    break;
                case NUMBER:
                    //we have number , number
                    tokens.insert(tokens.begin() + i + 1, "*");
                    break;
                case VARIABLE:
                    //we have number , x
                    tokens.insert(tokens.begin() + i + 1, "*");
                    break;
                case POWER:
                case MULTIPLY:
                case DIVIDE:
                case ADD:
                case SUBTRACT:
                case CLOSEP:
                    break;
                default:
                    //we have number , function
                    tokens.insert(tokens.begin() + i + 1, "*");
                    break;
                }
                break;
            case VARIABLE:
                switch (type(tokens[i + 1]))
                {
                case OPENP:
                    //we have x , "("
                    tokens.insert(tokens.begin() + i + 1, "*");
                    break;
                case NUMBER:
                    //we have x , number
                    tokens.insert(tokens.begin() + i + 1, "*");
                    break;
                case VARIABLE:
                    //we have x , x
                    tokens.insert(tokens.begin() + i + 1, "*");
                    break;
                case POWER:
                case MULTIPLY:
                case DIVIDE:
                case ADD:
                case SUBTRACT:
                case CLOSEP:
                    break;
                default:
                    //we have x , function
                    tokens.insert(tokens.begin() + i + 1, "*");
                    break;
                }
                break;
            }

        }
    }

    /// @brief Check to make sure brackets match up. Add extras if they don't
    /// @return function with added parens if needed
    std::string CheckBrackets(std::string& func) {
        int brackets = 0;
        for (int i = 0; i < func.length(); i++) {
            switch (func[i])
            {
            case '(':
                brackets++;
                break;
            case ')':
                brackets--;
                break;
            default:
                break;
            }
            if (brackets < 0) {
                func.erase(i, 1);
                i = -1;
                brackets = 0;
            }
        }
        while (brackets > 0) {
            func = func + ")";
            brackets--;
        }
        return func;
    }

    //TODO:
    //Make complicated funtions less complicated
    bool simplify() {
        return false;
    }

    /// @brief Change a vector of tokens to RPN using the shunting yard algoritm
    void parse_tokens(std::vector<std::string> tokens) {
        std::vector<unsigned short> operator_stack;
        while (tokens.size())
        {
            switch (type(tokens[0]))
            {
            case NUMBER:
                stack.push_back(NUMBER);
                if(tokens[0][0] == 'i')
                    number_stack.push_back(complex(0,1));
                else 
                    number_stack.push_back(stod(tokens[0]));
                tokens.erase(tokens.begin());
                break;
            case VARIABLE:
                stack.push_back(VARIABLE);
                number_stack.push_back(0);
                tokens.erase(tokens.begin());
                break;
            case POWER:
            case MULTIPLY:
            case DIVIDE:
            case ADD:
            case SUBTRACT:
                while (operator_stack.size() != 0) {
                    if (operator_stack.back() == OPENP) {
                        break;
                    }
                    if (type(tokens[0]) > operator_stack.back()) {
                        break;
                    }
                    stack.push_back(operator_stack.back());
                    number_stack.push_back(0);
                    operator_stack.pop_back();
                }
                operator_stack.push_back(type(tokens[0]));
                tokens.erase(tokens.begin());
                break;
            case OPENP:
                operator_stack.push_back(type(tokens[0]));
                tokens.erase(tokens.begin());
                break;
            case CLOSEP:
                if (operator_stack.size() == 0) {
                    throw(15);
                }
                while (operator_stack.back() != OPENP) {
                    stack.push_back(operator_stack.back());
                    number_stack.push_back(0);
                    operator_stack.pop_back();
                    if (operator_stack.size() == 0) {
                        break;
                    }
                }
                operator_stack.pop_back();
                if (operator_stack.size() == 0) {
                    tokens.erase(tokens.begin());
                    break;
                }
                if (operator_stack.back() >= SIN) {
                    stack.push_back(operator_stack.back());
                    number_stack.push_back(0);
                    operator_stack.pop_back();
                }
                tokens.erase(tokens.begin());
                break;
            default:
                operator_stack.push_back(type(tokens[0]));
                tokens.erase(tokens.begin());
                break;
            }
        }
        while (operator_stack.size() > 0)
        {
            stack.push_back(operator_stack.back());
            number_stack.push_back(0);
            operator_stack.pop_back();
        }
    }
    
    /// @brief Evalute a function in RPN using a recursive algorithm
    /// @param startPoint point to start at in the function
    /// @param input value to replace all of the variables with
    /// @return output of the function
    complex evaluateRPN(int startPoint, complex input) {
        if (stack[startPoint] == NUMBER) return number_stack[startPoint];
        if (stack[startPoint] == VARIABLE) return input;

        complex num1 = evaluateRPN(startPoint - 1, input);

        if (stack[startPoint] >= SIN) {
            switch (stack[startPoint])
            {
            case SIN:
                return sin(num1);
                break;
            case COS:
                return cos(num1);
                break;
            case TAN:
                return tan(num1);
                break;
            case SEC:
                return sec(num1);
                break;
            case CSC:
                return csc(num1);
                break;
            case COT:
                return cot(num1);
                break;
            }
        }
        int startPoint2 = startPoint - 2;
        int i = 0;

        if (stack[startPoint - 1] >= SIN) i += 1;
        else if (stack[startPoint - 1] >= ADD) i += 2;

        while (i > 0) {
            if (stack[startPoint2] >= SIN) i += 1;
            else if (stack[startPoint2] >= ADD) i += 2;
            startPoint2--;
            i--;
        }

        complex num2 = evaluateRPN(startPoint2, input);

        switch (stack[startPoint])
        {
        case POWER:
            throw 3;

            //return pow(num, num2);
            break;
        case MULTIPLY:
            return num2 * num1;
            break;
        case DIVIDE:
            return num2 / num1;
            break;
        case ADD:
            return num2 + num1;
            break;
        case SUBTRACT:
            return num2 - num1;
            break;
        default:
            throw 5;
        }
    }
};

#undef assert