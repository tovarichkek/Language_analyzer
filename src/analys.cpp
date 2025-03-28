
#include "analys.hpp"

//-------Symbol methods
bool Symbol::operator==(const Symbol& other) const {
    return type == other.type && value == other.value;
}
bool Symbol::operator<(const Symbol& other) const {
    if (type != other.type) 
        return type < other.type;
    return value < other.value;
}
std::string Symbol::toString() const {
    if (type == Symbol_type::Terminal) {
        switch(value) {
            case ID: return "id";
            case SUM: return "+";
            case SUB: return "-";
            case MUL: return "*";
            case DIV: return "/";
            case L_BRACK: return "(";
            case R_BRACK: return ")";
            case END_OF_LINE: return "$";
            default: return "?";
        }
    } else {
        switch(value) {
            case E_start: return "E'";
            case E: return "E";
            case T: return "T";
            case F: return "F";
            default: return "¿"; //lol
        }
    }
}

//-------Rule methods
bool Rule::operator==(const Rule& other) const {    //for set
    return lhs == other.lhs && rhs == other.rhs && dot_pos == other.dot_pos;
}

bool Rule::operator<(const Rule& other) const {     //for set
    if (lhs < other.lhs) {
        return true;
    }
    if (other.lhs < lhs) {
        return false;
    }
    if (dot_pos < other.dot_pos) {
        return true;
    }
    if (other.dot_pos < dot_pos) {
        return false;
    }
    return rhs < other.rhs;
}

std::string Rule::toString() const {
    std::string s = lhs.toString() + " → ";
    for (size_t i = 0; i < rhs.size(); ++i) {
        if (i == dot_pos) s += "•";
        s += rhs[i].toString();
    }
    if (dot_pos == rhs.size()) s += "•";
    return s;
}

//-------State methods
bool State::operator==(const State& other) const {
    return items == other.items;
}

//-------LR0 Parser methods
State LR0Parser::closure(const State& state) {
    State new_state = state;
    bool changed;
    do {
        changed = false;
        for (const auto& item : new_state.items) {
            if (item.dot_pos >= item.rhs.size()) continue;
            Symbol sym = item.rhs[item.dot_pos];
            if (sym.type == Symbol_type::NonTerminal) {
                for (const auto& rule : grammar.rules) {
                    if (rule.lhs == sym) {
                        Rule new_item{rule.lhs, rule.rhs, 0};
                        if (new_state.items.insert(new_item).second) {
                            changed = true;
                        }
                    }
                }
            }
        }
    } while (changed);
    return new_state;
}

State LR0Parser::go_to(const State& state, const Symbol& sym) {
    State new_state;
    for (const auto& item : state.items) {
        if (item.dot_pos < item.rhs.size() && item.rhs[item.dot_pos] == sym) {
            Rule new_item{item.lhs, item.rhs, item.dot_pos + 1};
            new_state.items.insert(new_item);
        }
    }
    return closure(new_state);
}

void LR0Parser::build_states() {
        Rule start_rule(
            Symbol(Symbol_type::NonTerminal, E_start), 
            {Symbol(Symbol_type::NonTerminal, E)}
        );
    
        State initial;
        initial.items.insert(start_rule);
        states.push_back(closure(initial));

    for (size_t i = 0; i < states.size(); ++i) {
        for (const auto& t : grammar.terminals) {
            State s = go_to(states[i], t);
            if (!s.items.empty()) {
                auto it = std::find(states.begin(), states.end(), s);
                if (it == states.end()) {
                    states.push_back(s);
                }
            }
        }
        for (const auto& nt : grammar.non_terminals) {
            State s = go_to(states[i], nt);
            if (!s.items.empty()) {
                auto it = std::find(states.begin(), states.end(), s);
                if (it == states.end()) {
                    states.push_back(s);
                }
            }
        }
    }
}

void LR0Parser::build_tables() {
    
    for (size_t state_id = 0; state_id < states.size(); ++state_id) {
        const auto& state = states[state_id];
        
        for (const auto& item : state.items) {
            if (item.dot_pos < item.rhs.size()) {
                Symbol sym = item.rhs[item.dot_pos];
                if (sym.type == Symbol_type::Terminal) {
                    State new_state = go_to(state, sym);
                    auto it = std::find(states.begin(), states.end(), new_state);
                    if (it != states.end()) {
                        int new_state_id = std::distance(states.begin(), it);
                        action_table[{state_id, sym}] = {SHIFT, new_state_id};
                    }
                }
            } else {
                if (item.lhs.value == E_start) { 
                    action_table[{state_id, Symbol(Symbol_type::Terminal, END_OF_LINE)}] = {ACCEPT, 0};
                } else {
                    int rule_index = -1;
                    for (size_t i = 0; i < grammar.rules.size(); ++i) {
                        const Rule& rule = grammar.rules[i];
                        if (rule.lhs == item.lhs && rule.rhs == item.rhs) {
                            rule_index = i;
                            break;
                        }
                    }
                    if (rule_index != -1) {
                        for (const auto& t : grammar.terminals) {
                            action_table[{state_id, t}] = {REDUCE, rule_index};
                        }
                    }
                    if (rule_index == -1) {
                        std::cerr << "ERROR: Rule not found for reduction: "
                                  << item.toString() << "\n";
                        continue;
                    }
                }
            }
        }
        
        for (const auto& nt : grammar.non_terminals) {
            State new_state = go_to(state, nt);
            auto it = std::find(states.begin(), states.end(), new_state);
            if (it != states.end()) {
                int new_state_id = std::distance(states.begin(), it);
                goto_table[{state_id, nt}] = new_state_id;
            }
        }
    }
}

void LR0Parser::print_tables() {
    std::cout << "ACTION Table:\n";
    for (const auto& [key, val] : action_table) {
        std::cout << "State " << key.first 
                  << ", Symbol " << key.second.toString() 
                  << ": ";
        switch(val.type) {
            case SHIFT: std::cout << "Shift " << val.value; break;
            case REDUCE: std::cout << "Reduce " << val.value; break;
            case ACCEPT: std::cout << "Accept"; break;
        }
        std::cout << "\n";
    }
    
    std::cout << "\nGOTO Table:\n";
    for (const auto& [key, val] : goto_table) {
        std::cout << "State " << key.first 
                  << ", NonTerminal " << key.second.toString() 
                  << ": " << val << "\n";
    }
}

void LR0Parser::print_states() {
    std::cout << "\n=== STATES ===\n";
    for (size_t i = 0; i < states.size(); ++i) {
        std::cout << "\nState " << i << ":\n";
        for (const auto& item : states[i].items) {
            std::cout << "  " << item.toString() << "\n";
        }
    }
    std::cout << "==============\n\n";
}



//------------------------------ГЛАВНОЕ НЕ ПАНИКОВАТЬ----------------------
void LR0Parser::parse(std::deque<Symbol>& input_queue) {

    std::deque<Symbol> input = input_queue;

    std::stack<std::pair<int, Symbol>> state_stack;
    state_stack.push({0, Symbol{Symbol_type::Terminal, END_OF_LINE}});

    print_header();
    size_t step = 0;

    while (!input.empty()) {
        std::cout << std::endl;
        print_step(++step, state_stack, input);
        const auto& [current_state, _] = state_stack.top();
        Symbol current_symbol = input.front();

        auto action = get_action(current_state, current_symbol);
        if (!action.has_value()) {
            std::cout << " ERROR: No action found\n";
            return;
        }

        switch (action->type) {
            case SHIFT: 
                handle_shift(state_stack, input, action->value); 
                break;
            case REDUCE: 
                if (!handle_reduce(state_stack, action->value)) return;
                break;
            case ACCEPT: 
                handle_accept(); 
                return;
        }
    }
}

std::optional<LR0Parser::Action> LR0Parser::get_action(int state, Symbol symbol) {
    auto it = action_table.find({state, symbol});
    if (it == action_table.end()) {
        std::cout << "ERROR: No action for state " << state << " and symbol " << symbol.toString() << "\n";
        return {};
    }
    return it->second;
}

void LR0Parser::handle_shift(std::stack<std::pair<int, Symbol>>& stack, std::deque<Symbol>& input, int new_state) {
    std::cout << "Shift to " << new_state;
    stack.push({new_state, input.front()});
    input.pop_front();
}

bool LR0Parser::handle_reduce(std::stack<std::pair<int, Symbol>>& stack, int rule_id) {
    const Rule& rule = grammar.rules[rule_id];
    std::cout << "Reduce " << rule.toString();

    if (stack.size() < rule.rhs.size() ) {
        std::cout << " ERROR: Not enough elements in stack (need " 
                  << rule.rhs.size() << ", have " << stack.size() << ")\n";
        return false;
    }

    for (size_t i = 0; i < rule.rhs.size(); ++i) {
        if (stack.empty()) {
            std::cout << " ERROR: Stack underflow\n";
            return false;
        }
        stack.pop();
    }

    if (stack.empty()) {
        std::cout << " ERROR: Stack empty after reduction\n";
        return false;
    }
    int new_state = stack.top().first;

    auto goto_it = goto_table.find({new_state, rule.lhs});
    if (goto_it == goto_table.end()) {
        std::cout << " ERROR: No GOTO for " << rule.lhs.toString() 
                  << " in state " << new_state << "\n";
        return false;
    }

    stack.push({goto_it->second, rule.lhs});
    return true;
}

void LR0Parser::handle_accept() {
    std::cout << "Accept\n";
}

void LR0Parser::print_header() const {
    std::cout << "\nParsing steps:\n";
    std::cout << std::left
              << std::setw(6) << "Step" 
              << std::setw(30) << "Stack (states)" 
              << std::setw(40) << "Stack (symbols)" 
              << std::setw(40) << "Input" 
              << "Action\n";
    std::cout << std::string(150, '-') << "\n";
}

void LR0Parser::print_step(size_t step, const std::stack<std::pair<int, Symbol>>& stack, const std::deque<Symbol>& input) {
    std::cout << std::setw(6) << step;
    auto temp_states = stack;
    std::vector<int> states_vec;
    while (!temp_states.empty()) {
        states_vec.push_back(temp_states.top().first);
        temp_states.pop();
    }
    std::reverse(states_vec.begin(), states_vec.end());
    std::stringstream ss_states;
    for (auto s : states_vec) ss_states << s << " ";
    std::cout << std::setw(30) << ss_states.str();

    auto temp_symbols = stack;
    std::vector<Symbol> symbols_vec;
    while (!temp_symbols.empty()) {
        symbols_vec.push_back(temp_symbols.top().second);
        temp_symbols.pop();
    }
    std::reverse(symbols_vec.begin(), symbols_vec.end());
    std::stringstream ss_symbols;
    for (const auto& sym : symbols_vec) ss_symbols << sym.toString() << " ";
    std::cout << std::setw(40) << ss_symbols.str();

    std::stringstream ss_input;
    for (const auto& sym : input) ss_input << sym.toString() << " ";
    std::cout << std::setw(40) << ss_input.str();
}