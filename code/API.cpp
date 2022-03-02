/*
 * @Date        : 2021-06-01
 * @Author      : your name
 * @FilePath    : \MiniSQL\TEST\API.cpp
 * @Description : 处理输入的SQL命令，判断并调用各个数据处理的接口函数
 */

#ifndef API_CPP
#define API_CPP

#include "API.hpp"

/**
 * @description: 将读入的字符串进行分析处理，
 *               转换为SQLCommand类，并判断操作，
 *               传入到响应的功能函数中，完成对数据库的操作
 * @param {string} sql_query
 * @return {*}
 */
bool API::execute()
{
    clock_t start, finish;
    double duration;

    start = clock();
    interface(this->sql_command);
    finish = clock();
    static int count;
    duration = (double)(finish - start) / CLOCKS_PER_SEC;
    printf("(%d Executing tiem:%f seconds )\n", ++count, duration);

    // this->sql_command.print();
    this->sql_command.clear();
    return true;
}

void API::clear()
{
    this->sql_command.clear();
}

bool API::set_command_type(int sql_command)
{
    this->sql_command.command_type = sql_command; /** command type：
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
    return true;
}

bool API::set_file_name(string file_name)
{
    this->sql_command.file_name = file_name;
    return true;
}

bool API::set_table_name(string table_name)
{
    this->sql_command.table_name = table_name;
    return true;
}

bool API::set_index(string index_name, string col_name)
{
    this->sql_command.index_num++;
    this->sql_command.index_name = index_name;
    this->sql_command.col_name = col_name;
    return true;
}

bool API::set_attributes(string attri_name, string data_type, char constraint)
{
    this->sql_command.attri_num++;
    this->sql_command.attri_name.push_back(attri_name);
    this->sql_command.attri_datatype.push_back(data_type);
    this->sql_command.attri_keytype[this->sql_command.attri_num - 1] = constraint;
    return true;
}

bool API::set_primary_key(string attri_name)
{
    int index = 0;
    vector<string>::iterator i;
    for (i = this->sql_command.attri_name.begin(); i != this->sql_command.attri_name.end(); i++)
    {
        if (*i == attri_name)
        {
            this->sql_command.attri_keytype[index] = 'p';
            break;
        }
        index++;
    }

    return true;
}

bool API::set_values(string data_type, string value)
{
    this->sql_command.value_num++;
    this->sql_command.value_datatype.push_back(data_type);
    this->sql_command.value.push_back(value);
    return true;
}

bool API::set_conditions(string condi_attri_name, int condi_operator, string condi_data, string condi_datatype)
{
    this->sql_command.condition_num++;
    this->sql_command.condi_attri_name.push_back(condi_attri_name);

    /**condi_operator
     * "="	  = 1;
     * ">="	  = 2;
     * ">"	  = 3; 
     * "<="	  = 4;
     * "<"	  = 5;
     * "!=" | "<>" = 6
     */
    switch (condi_operator)
    {
    case 1:
        this->sql_command.condi_operator.push_back("=");
        break;
    case 2:
        this->sql_command.condi_operator.push_back(">=");
        break;
    case 3:
        this->sql_command.condi_operator.push_back(">");
        break;
    case 4:
        this->sql_command.condi_operator.push_back("<=");
        break;
    case 5:
        this->sql_command.condi_operator.push_back("<");
        break;
    case 6:
        this->sql_command.condi_operator.push_back("!=");
        break;
    default:
        break;
    }
    this->sql_command.condi_data.push_back(condi_data);
    this->sql_command.condi_datatype.push_back(condi_datatype);

    return true;
}

int API::get_attri_num()
{
    return sql_command.attri_num;
}

#endif