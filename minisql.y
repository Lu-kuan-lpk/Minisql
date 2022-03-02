%{
    #include <stdio.h>
    #include <string>
    #include <stdarg.h>
    #include "API.hpp"

    extern "C"{
        void yyerror(char *s,...);
        int yylex(void);
        extern int yylineno;
        extern FILE* yyin;
    };

    API api; // analyze the sql command and recall the corresponding function
    clock_t execfile_starttime, execfile_finishtime;
    int fileinput = 0;  // - 1: yyin is from the specific file; - 0: yyin is from stdin 
%}

%union {
	int intval;
	double floatval;
	char *strval;
	int compare_op;
    char* condition;
}

%token <strval> NAME FILENAME
%token <strval> ERROR_STR
%token <strval> STRING
%token <intval> INT_NUM
%token <floatval> FLOAT_NUM

%token CREATE
%token DROP
%token SELECT
%token INSERT
%token DELETE
%token QUIT
%token EXECFILE

%token TABLE
%token INDEX
%token PRIMARY
%token KEY
%token VALUES

%token FROM
%token WHERE
%token INTO
%token ON
%token UNIQUE
%token AND
%token ENDL

%token CHAR
%token INT
%token FLOAT

%type <intval> data_type
%type <strval> insert_val 
%token <compare_op> COMPARISON
%type <condition> where_clause condition

%left  COMPARISON
%start statements

%%

statements: statement ';' 
| statements statement ';' ;

statement: create_table ';'     { api.execute(); return 0; }
| drop_table ';'                { api.execute(); return 0; }
| create_index ';'              { api.execute(); return 0; }
| drop_index ';'                { api.execute(); return 0; }
| select_statement ';'          { api.execute(); return 0; }
| insert_statement ';'          { api.execute(); return 0; }
| delete_statement ';'          { api.execute(); return 0; }
| QUIT ';'                      { api.set_command_type(8); api.execute(); exit(1);}
| execfile_statement ';'        { return 0; };

create_table: CREATE TABLE NAME '(' attri_list ')'
    {
        api.set_command_type(1);
        api.set_table_name($3);
        free($3);
    };
drop_table: DROP TABLE NAME
    {
        api.set_command_type(2);
        api.set_table_name($3);
        free($3);
    };
create_index: CREATE INDEX NAME ON NAME '(' NAME ')'
    {
        api.set_command_type(3);
        api.set_table_name($5);
        api.set_index($3, $7);
        free($3);
        free($5);
        free($7);
    };
drop_index: DROP INDEX NAME
    {
        api.set_command_type(4);
        api.set_index($3, "");
        free($3);
    };
select_statement: SELECT '*' FROM NAME where_clause
    {
        api.set_command_type(5);
        api.set_table_name($4);
        free($4);
    };
insert_statement: INSERT INTO NAME VALUES '(' insert_vallist ')'
    {
        api.set_command_type(6);
        api.set_table_name($3);
        free($3);
    };
delete_statement: DELETE FROM NAME where_clause
    {
        api.set_command_type(7);
        api.set_table_name($3);
        free($3);
    };
execfile_statement: EXECFILE FILENAME
    {
        api.set_command_type(9);
        string filename($2);

        FILE *fp = fopen(filename.c_str(),"r");

        if(fp == NULL){
            yyerror("Cannot open the file %s.", $2);return 0;}
        else 
        {
            printf("Executing......\n");
            execfile_starttime = clock();
            fileinput = 1;
            yyin = fp;
        }
        return 0;
    };

attri_list: attri_definition           {;}
    | attri_list ',' attri_definition  {;};
attri_definition: NAME data_type
        {
            if(api.get_attri_num() > 32){
                        yyerror("Input too many attributes."); return 0;}
            switch ($2){
                case 1: api.set_attributes($1, "int", 'k'); break;
                case 2: api.set_attributes($1, "float", 'k'); break;
                default: api.set_attributes($1, "char"+to_string($2 - 3), 'k'); 
            }
            free($1); 
        }
    | NAME data_type attri_constraint
        {
            if(api.get_attri_num() > 32){
                        yyerror("Input too many attributes.");
                        return 0;}
            switch ($2){
                case 1: api.set_attributes($1, "int", 'u'); break;
                case 2: api.set_attributes($1, "float", 'u'); break;
                default: int len = $2; api.set_attributes($1, "char"+to_string(len - 3), 'u'); 
            }
            free($1); 
        }
    | PRIMARY KEY '(' NAME ')'    { api.set_primary_key($4); free($4); };
data_type: INT                  { $$ = 1; }
    | FLOAT                     { $$ = 2; }
    | CHAR '(' INT_NUM ')'
                {
                    if($3 > 255) {yyerror("The length of type 'char' is too long."); return 0;}
                    else if($3 < 1) {yyerror("THe length of type 'char' must be positive."); return 0;}
                    else { $$ = 3 + $3; }
                };

attri_constraint: UNIQUE {;};

insert_vallist:  insert_val        {;}
    | insert_vallist ',' insert_val    {;};
insert_val: INT_NUM    
                {
                    if(api.get_attri_num() > 32){
                        yyerror("Input too many values.");
                        return 0;}
                    else
                    {
                        api.set_values("int", to_string($1));
                    }
                }
        | FLOAT_NUM
                {
                    if(api.get_attri_num() > 32){
                        yyerror("Input too many values."); return 0;}
                    else
                    {
                        api.set_values("float", to_string($1));
                    }
                }
        | STRING
                {
                    if(api.get_attri_num() > 32){
                        yyerror("Input too many values.");return 0;}
                    else
                    {
                        api.set_values("char",$1);
                        free($1);
                    }
                };

where_clause: WHERE condition_list
    {;}
|   {;};
condition_list: condition           {;}
| condition_list AND condition      {;};
condition: NAME COMPARISON INT_NUM  {api.set_conditions($1, $2, to_string($3), "int");}
| NAME COMPARISON FLOAT_NUM             {api.set_conditions($1, $2, to_string($3), "float");}
| NAME COMPARISON STRING            {api.set_conditions($1, $2, $3, "char"); free($3);};

%%

void yyerror(char *s,...){
    api.clear();
    va_list ap;
    va_start(ap, s);
    fprintf(stderr, "Error on line %d: ", yylineno);
    vfprintf(stderr, s, ap);
    fprintf(stderr, "\n");
    va_end(ap);
}


int main(int argc, const char * argv[])
{
    cout << "*********************************************************" << endl;
    cout << "*********************************************************" << endl;
    cout << "************       Welcome to minisql!        ***********" << endl;
    cout << "*********************************************************" << endl;
    cout << "*********************************************************" << endl;
    cout << "************        Create by: Lu Kuan        ***********" << endl;
    cout << "************                   Lu Kaixuan     ***********" << endl;
    cout << "************                   Gao Jing       ***********" << endl;
    cout << "************                   Mao Qinyue     ***********" << endl;
    cout << "*********************************************************" << endl;
    cout << "*********************************************************" << endl;
    cout << "************        Copyright (C) 2021        ***********" << endl;
    cout << "*********************************************************" << endl;
    cout << "*********************************************************" << endl<<endl;
    while (true) {
        if(fileinput == 0)
            cout << "minisql>";
        yyparse();
    }
    return 0;
}
