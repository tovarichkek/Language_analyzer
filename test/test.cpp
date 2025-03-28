#include <gtest/gtest.h>
#include <FlexLexer.h>
#include <fstream>
#include <sstream>

#include "analys.hpp"
#include "tokens.h"

class ParserTest : public ::testing::Test {
protected:
    void SetUp() override {
        grammar.rules = {
            Rule(
                Symbol(Symbol_type::NonTerminal, E), 
                {
                    Symbol(Symbol_type::NonTerminal, E), 
                    Symbol(Symbol_type::Terminal, SUM), 
                    Symbol(Symbol_type::NonTerminal, T)
                }
            ),
            Rule(
                Symbol(Symbol_type::NonTerminal, E), 
                {
                    Symbol(Symbol_type::NonTerminal, E), 
                    Symbol(Symbol_type::Terminal, SUB), 
                    Symbol(Symbol_type::NonTerminal, T)
                }
            ),
            Rule(
                Symbol(Symbol_type::NonTerminal, E), 
                { 
                    Symbol(Symbol_type::NonTerminal, T)
                }
            ),
            Rule(
                Symbol(Symbol_type::NonTerminal, T), 
                {
                    Symbol(Symbol_type::NonTerminal, T), 
                    Symbol(Symbol_type::Terminal, MUL), 
                    Symbol(Symbol_type::NonTerminal, T)
                }
            ),    
            Rule(
                Symbol(Symbol_type::NonTerminal, T), 
                {
                    Symbol(Symbol_type::NonTerminal, T), 
                    Symbol(Symbol_type::Terminal, DIV), 
                    Symbol(Symbol_type::NonTerminal, T)
                }
            ),    
            Rule(
                Symbol(Symbol_type::NonTerminal, T), 
                {
                    Symbol(Symbol_type::NonTerminal, F), 
                }
            ),    
            Rule(
                Symbol(Symbol_type::NonTerminal, F), 
                {
                    Symbol(Symbol_type::Terminal, ID), 
                }
            ), 
    
            Rule(
                Symbol(Symbol_type::NonTerminal, F), 
                {
                    Symbol(Symbol_type::Terminal, L_BRACK), 
                    Symbol(Symbol_type::NonTerminal, E), 
                    Symbol(Symbol_type::Terminal, R_BRACK), 
                }
            ), 
        };
    
        grammar.terminals = {
            Symbol(Symbol_type::Terminal, ID),
            Symbol(Symbol_type::Terminal, SUM),
            Symbol(Symbol_type::Terminal, SUB),
            Symbol(Symbol_type::Terminal, MUL),
            Symbol(Symbol_type::Terminal, DIV),
            Symbol(Symbol_type::Terminal, L_BRACK),
            Symbol(Symbol_type::Terminal, R_BRACK),
            Symbol(Symbol_type::Terminal, END_OF_LINE),
        };
    
        grammar.non_terminals = {
            Symbol(Symbol_type::NonTerminal, E_start),  //E' is S in this sheme
            Symbol(Symbol_type::NonTerminal, E),
            Symbol(Symbol_type::NonTerminal, T),
            Symbol(Symbol_type::NonTerminal, F)
        };
    }

    void run_test(const std::string& input, bool should_pass, int test_num) {
        std::deque<Symbol> symbols = parse_input(input);
        testing::internal::CaptureStdout();
        
        LR0Parser parser(grammar);
        parser.parse(symbols);
        
        std::string output = testing::internal::GetCapturedStdout();
        save_test_result(output, test_num);

        if(should_pass) {
            EXPECT_TRUE(output.find("Accept") != std::string::npos) << "Parsing failed for valid input";
        } else {
            EXPECT_TRUE(output.find("ERROR") != std::string::npos) << "Parsing succeeded for invalid input";
        }
    }

private:
    std::deque<Symbol> parse_input(const std::string& input) {
        std::deque<Symbol> symbols;
        yyFlexLexer lexer;
        
        std::istringstream input_stream(input);
        lexer.switch_streams(&input_stream, nullptr);
        
        int token;
        while((token = lexer.yylex()) != END_OF_LINE) {
            symbols.emplace_back(Symbol_type::Terminal, token);
        }

        symbols.emplace_back(Symbol_type::Terminal, END_OF_LINE);
        
        return symbols;
    }

    void save_test_result(const std::string& result, int test_num) {
        //PROJECT_PATH is in the Cmake
        std::ofstream out(PROJECT_PATH + std::string("/test/out_of_tests/ ") + "test_num_" + std::to_string(test_num) + ".txt");
        out << result;
        out.close();
    }

    LR0Parser::Grammar grammar;
};

TEST_F(ParserTest, ValidExpression) {
    run_test("x + y\n", true, 1);
}

TEST_F(ParserTest, InvalidToken) {
    run_test("x - y\n", true, 2);
}

TEST_F(ParserTest, ComplexExpression) {
    run_test("(123+45-2 ) * z / 6 \n", true, 3);
}

TEST_F(ParserTest, SimpleAddition) {
    run_test("(x + y\n", false, 4);
}

TEST_F(ParserTest, SimpleMultiplication) {
    run_test("x ** y\n", false, 5);
}

TEST_F(ParserTest, ParenthesesWithAddition) {
    run_test(" (x + y) \n", true, 6);
}

TEST_F(ParserTest, ComplexExpressionWithPriority) {
    run_test("x + y * z\n", true, 7);
}

TEST_F(ParserTest, DivisionAndSubtraction) {
    run_test("x / y - z\n", true, 8);
}

TEST_F(ParserTest, NestedParentheses) {
    run_test("(x + (y * z)) \n", true, 9);
}

TEST_F(ParserTest, UnclosedParenthesis) {
    run_test("(x + y) \n", true, 10);
}

TEST_F(ParserTest, EmptyExpression) {
    run_test(" \n", false, 11);
}

TEST_F(ParserTest, MultipleOperations) {
    run_test("x + y * z - a / b\n", false, 12);
}

TEST_F(ParserTest, IncorrectParenthesisOrder) {
    run_test("x + (y * z)\n", true, 13);
}

TEST_F(ParserTest, ExpressionWithSpaces) {
    run_test("  x   +  y  \n", true, 14);
}

TEST_F(ParserTest, SingleNumber) {
    run_test("42\n", true, 15);
}

TEST_F(ParserTest, SingleVariable) {
    run_test("x\n", true, 16);
}

TEST_F(ParserTest, InvalidNumberFormat) {
    run_test("12a3 + 4\n", false, 17);
}

TEST_F(ParserTest, MultipleParentheses) {
    run_test("((x + y) * (z - a))\n", false, 18);
}

TEST_F(ParserTest, NegativeNumber) {
    run_test("x + (-5)\n", false, 19);
}

TEST_F(ParserTest, ExtraOperators) {
    run_test("x + y\n", true, 20);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}