#pragma once

enum TERMINALS{


    // Операторы
    SUM,
    SUB,
    MUL,
    DIV,
    
    // Переменные
    ID,
    
    // Скобки
    L_BRACK,
    R_BRACK,
    
    // Специальные
    END_OF_LINE,
    END_OF_FILE,

    ERROR
};

enum NONTERMINALS{
    E_start,
    E,
    F,
    T
};

extern int yylval;  // Для передачи значений чисел