#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <algorithm>

#include <iomanip>  //setw
#include <stack>
#include <deque> 
#include "tokens.h"
#include <optional>


//extern int yylex();
//extern char* yytext;

enum class Symbol_type {
    Terminal, 
    NonTerminal 
};

class Symbol {
public:
    Symbol_type type;
    int value;

    Symbol(Symbol_type t, int v) : type(t), value(v) {}
    
    bool operator==(const Symbol& other) const;
    bool operator<(const Symbol& other) const;
    
    std::string toString() const;
};

class Rule {
public:
    Symbol lhs; //left part
    std::vector<Symbol> rhs; //right part
    size_t dot_pos; //num of dot position in rhs

    Rule(Symbol l, std::vector<Symbol> r, size_t d = 0) : lhs(l), rhs(r), dot_pos(d) {}

    bool operator==(const Rule& other) const;
    bool operator<(const Rule& other) const;
    
    std::string toString() const;
};

class State {
public:
    std::set<Rule> items;

    bool operator==(const State& other) const;
};

class LR0Parser {
public:
    enum ActionType { SHIFT, REDUCE, ACCEPT };
    struct Action {
        ActionType type;
        int value;
    };

    struct Grammar {
        std::vector<Rule> rules;
        std::set<Symbol> terminals;
        std::set<Symbol> non_terminals;
    };

private:
    Grammar grammar;
    std::vector<State> states;
    std::map<std::pair<int, Symbol>, Action> action_table;
    std::map<std::pair<int, Symbol>, int> goto_table;

    State closure(const State& state);

    State go_to(const State& state, const Symbol& sym);

    void build_states();
    void build_tables() ;

public:
    LR0Parser(const Grammar& g) : grammar(g) {
        build_states();
        build_tables();
    }

    void print_tables();
    void print_states();



    void parse( std::deque<Symbol>& input);
private:
    std::optional<Action> get_action(int state, Symbol symbol);
    void handle_shift(std::stack<std::pair<int, Symbol>>& stack, std::deque<Symbol>& input, int new_state);
    bool handle_reduce(std::stack<std::pair<int, Symbol>>& stack, int rule_id);
    void handle_accept();
    void print_header() const;
    void print_step(size_t step, const std::stack<std::pair<int, Symbol>>& stack, const std::deque<Symbol>& input);


};