%{
    #include <iostream>
    #include <time.h>
    #include "y.tab.h"
    
    extern "C" {
        int yylex(void);
        int yywrap(void);
        void yyerror(char *s, ...);
    }

    extern clock_t execfile_starttime, execfile_finishtime;
    extern int fileinput;
%}

%option yylineno

%%

    /* define keywords */
create	    {return CREATE; }
drop	    {return DROP;}
select      {return SELECT;}
insert	    {return INSERT;}
delete	    {return DELETE;}
quit	    {return QUIT;}
execfile	{return EXECFILE;}

table	    {return TABLE;}
index	    {return INDEX;}
primary	    {return PRIMARY;}
key	        {return KEY;}
values	    {return VALUES;}

from	    {return FROM;}
where	    {return WHERE;}
into	    {return INTO;}
on	        {return ON;}
unique	    {return UNIQUE;}
"and"	        {return AND;}

    /* define value type */
"char"	    {return CHAR;}
"int" 	    {return INT;}
"float"	    {return FLOAT;}

    /* define comparison operators */
"="	        { yylval.compare_op = 1; return COMPARISON; }
">="	    { yylval.compare_op = 2; return COMPARISON; }
">"	        { yylval.compare_op = 3; return COMPARISON; }
"<="	    { yylval.compare_op = 4; return COMPARISON; }
"<"	        { yylval.compare_op = 5; return COMPARISON; }
"!=" | "<>"	{ yylval.compare_op = 6; return COMPARISON; } 

[-+&~|^/%*(),.;!\\]   { return yytext[0]; }

    /* define digit numbers */
-?[0-9]+            { yylval.intval = atoi(yytext); return INT_NUM;}
-?[0-9]+\.[0-9]+    { yylval.floatval = atof(yytext); return FLOAT_NUM;}

    /* define string */
'(\\.|''|[^'\n])*'	|
\"(\\.|\"\"|[^"\n])*\" { yylval.strval = strdup(yytext); return STRING;}

'(\\.|[^'\n])*$	       { yyerror("Unterminated string: %s", yytext); return ERROR_STR; }
\"(\\.|[^"\n])*$       { yyerror("Unterminated string: %s", yytext); return ERROR_STR; }

    /* define name and filename */
[A-Za-z][A-Za-z0-9_]*	{ yylval.strval = strdup(yytext); return NAME; }
[/A-Za-z0-9_-]+\.[A-Za-z]+ { yylval.strval = strdup(yytext); return FILENAME; }

[ \t]+                   ;/* whitespace */
.                       { yyerror("Invalid character: '%c'.", yytext); return ERROR_STR; }
[\n]	                { if(fileinput == 0) printf( ">>> ");}

%%

int yywrap(void)
{
    if(fileinput == 1)
    {
        double file_duration;
        execfile_finishtime = clock();
        file_duration = (double)(execfile_finishtime - execfile_starttime) / CLOCKS_PER_SEC;
        printf("Execute file successfully(%f sounds)! Please input next command...\nminisql> ", file_duration);
    }
    fileinput = 0;
    yyin=stdin;
    return 0;
}