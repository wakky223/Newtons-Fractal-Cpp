#include "function.hpp"

/// @brief init function object
/// @param funcString function string
void func::init(std::string funcString) {
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
void func::init() {
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