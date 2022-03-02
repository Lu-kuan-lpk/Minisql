/*
 * @Date        : 2021-06-01
 * @Author      : your name
 * @FilePath    : \mini_sql\MiniSQL\Interpreter\SQLCommand.hpp
 * @Description : SQL命令类，用于记录SQL命令的类型，以及所提供的参数
 */

#ifndef SQLCOMMAND_HPP
#define SQLCOMMAND_HPP

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <string.h>
#include <vector>
using namespace std;

class SQLCommand
{
public:
    int command_type; /** command type：
                        * - 0：error command
                        * - 1：create table
                        * - 2：drop table
                        * - 3：create index
                        * - 4：drop index
                        * - 5：select * from ... (where) ...
                        * - 6：insert into ... values
                        * - 7：delete from ... (where) ...
                        * - 8：quit
                        * - 9：execfile
                        */

    string file_name; // only for command: execfile file_name;
    string table_name;

    // for command: create index index_name on table_name ( col_name );
    int index_num; // 1 or 0
    string index_name;
    string col_name;

    /** for command: 
     * create table table_name (
     * 	attri_name attri_name ,
     * 	attri_name attri_name ,
     * 	attri_name attri_name ,
     * 	primary key ( attri_name )
     * );
     * 
     * e.g.: 
     * create table student (
     * 		sno char(8),
     * 		sname char(16) unique,
     * 		sage int,
     * 		sgender char (1),
     * 		primary key ( sno )
     * );
     * 
     * then: attri_num = 4
     *      attri_keytype = "pukk"
     * <vector> attri_name = "sno", "sname", "sang", "sgender"
     * <vector> attri_datatype = "char8", "char16", "int", "char1"
     */
    int attri_num;
    char attri_keytype[32]; /** represent the key type: 
                                - ordinary key: 'k'; 
                                - primary key: 'p'; 
                                - unique key: 'u'
                            */
    vector<string> attri_name;
    vector<string> attri_datatype; // "int", "float", "char"+length; e.g. "char128" means char(128)

    /** for command: insert into table_name values ( value1 , value2 , … , valuen );
     * e.g.
     * insert into student values ('12345678',’wy’,22,’M’); 
     * then:
     * <vector> value = "12345678", "wy", 22, "M"
     * <vector> value_datatype = "char", "char", "int", "char"
     */
    int value_num;
    vector<string> value;
    vector<string> value_datatype; // "int", "char", "float"

    /** for where clause:
     * e.g.
     * select * from student where sage > 20 and sgender = 'F'; 
     * then:
     * condition_num = 2
     * <vector> condi_attri_name = "sage", "sgender"
     * <vector> condi_operator = ">", "="
     * <vector> condi_data = 20, "F"
     * <vector> condi_datatype = "int", "char"
     */
    int condition_num;
    vector<string> condi_attri_name;
    vector<string> condi_operator;
    vector<string> condi_data;
    vector<string> condi_datatype;

    SQLCommand();

    /**
     * @description: clear all sql information in the instance, and wait for the next sql command
     * @param {*}
     * @return {*}
     */
    void clear();

    /**
     * @description: just for testing, to see all info in this class
     * @param {*}
     * @return {*}
     */
    void print();
};

#endif