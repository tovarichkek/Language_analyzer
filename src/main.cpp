#include <FlexLexer.h>
#include "analys.hpp"

extern int yylex();
extern char* yytext;


int main() {
    LR0Parser::Grammar grammar;

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

    LR0Parser parser(grammar);
    //parser.print_tables();
    //parser.print_states();

   
    

    yyFlexLexer lexer;
    std::deque<Symbol> input_symbols;

    int token;
    while ((token = lexer.yylex()) != END_OF_LINE) {
        input_symbols.push_back(Symbol(Symbol_type::Terminal, (token)));
    }
    input_symbols.emplace_back(Symbol_type::Terminal, END_OF_LINE);

    for(auto x:input_symbols){
        std::cout << x.toString();
    }
    std::cout << std::endl << std::endl;

    parser.parse(input_symbols);

    return 0;
}