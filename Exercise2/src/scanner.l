%{
#include "vslc.h"

// The tokens defined in parser.y
#include "parser.h"

// parser.h contains some unused functions, ignore that
#pragma GCC diagnostic ignored "-Wunused-function"
%}

%option noyywrap
%option array
%option yylineno

WHITESPACE [ |\t|\n|\r|\v]
COMMENT \/\/[^\n]+
QUOTED \"([^\"\n]|\\\")*\"

FUNC func
VAR var
RETURN return
PRINT print
IF if
THEN then
ELSE else
WHILE while
DO do
BREAK break
OPERATOR \+|\-|\*|\/|[\=]|[\!]|\<|[\<]|\>|[\>]|[\-]|[\!]
NUMBER [0-9]+
IDENTIFIER [a-zA-Z_][a-zA-Z0-9_]*
STRING "."


%%
{WHITESPACE}+           { /* Eliminate whitespace */ }
{COMMENT}               { /* Eliminate comments */ }
{QUOTED}                { return STRING_TOKEN; }
,                       {return ',';}
"["    { return '['; }
"]"    { return ']'; }
"("    { return '('; }
")"    { return ')'; }


"{"    { return '{'; }
"}"    { return '}'; }

{FUNC}                  {return FUNC;}
{VAR}                  {return VAR;}
{RETURN}                  {return RETURN;}
{PRINT}                  {return PRINT;}
{IF}                  {return IF;}
{THEN}                  {return THEN;}
{ELSE}                  {return ELSE;}
{WHILE}                  {return WHILE;}
{DO}                  {return DO;}
{BREAK}                  {return BREAK;}

{OPERATOR}              {return yytext[0];}
{NUMBER}                {return NUMBER_TOKEN;}
{IDENTIFIER}            {return IDENTIFIER_TOKEN;}
{STRING}                {return STRING_TOKEN;}
%%
