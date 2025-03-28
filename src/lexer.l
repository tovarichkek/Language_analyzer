%{
#include <iostream>
#include "../include/tokens.h"
%}

%option noyywrap
%option c++

%%
[0-9]           { return ID; }
[1-9][0-9]*     { return ID; }

"+"             { return SUM; }        
"*"             { return MUL; }           
"-"             { return SUB; }           
"/"             { return DIV; }           

"x"             { return ID; }             
"y"             { return ID; }             
"z"             { return ID; }             

"("             { return L_BRACK; }       
")"             { return R_BRACK; }       

\n              { return END_OF_LINE; }   

[ \t]           ; // Ignore spaces and tabs

.               { return ERROR; }

%%