/*
 * @Date        : 2021-06-01
 * @Author      : your name
 * @FilePath    : \MiniSQL\TEST\API.hpp
 * @Description : 处理输入的SQL命令，判断并调用各个数据处理的接口函数
 */

#ifndef API_HPP
#define API_HPP

#include <time.h>
#include <algorithm>
#include "SQLCommand.hpp"
#include "interface.h"

class API
{
private:
    SQLCommand sql_command;

public:
    bool set_command_type(int sql_type);
    bool set_file_name(string file_name);
    bool set_table_name(string table_name);
    bool set_index(string index_name, string col_name);
    bool set_attributes(string attri_name, string data_type, char constraint);
    bool set_primary_key(string attri_name);
    bool set_values(string data_type, string value);
    bool set_conditions(string condi_attri_name, int condi_operator, string condi_data, string condi_datatype);
    int get_attri_num();

    void clear();
    bool execute();
};

#endif