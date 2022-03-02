/*
 * @Date        : 2021-06-01
 * @Author      : your name
 * @FilePath    : \undefinedd:\Shool_Work\Sophomore_4\Database_System\Labs\mini_sql\MiniSQL\Interpreter\SQLCommand.cpp
 * @Description : SQL命令类，用于记录SQL命令的类型，以及所提供的参数
 */

#ifndef SQLCOMMAND_CPP
#define SQLCOMMAND_CPP
#include "SQLCommand.hpp"

SQLCommand::SQLCommand()
{
    table_name = "";
    index_name.clear();
    attri_num = 0;
    attri_name.clear();
    value_num = 0;
    value.clear();
    file_name = "";
}

/**
 * @description: clear all sql information in the instance, wait for the next sql command
 * @param {*}
 * @return {*}
 */
void SQLCommand::clear()
{
    file_name = "";
    table_name = "";

    index_num = 0;
    index_name.clear();
    col_name.clear();

    attri_num = 0;
    strcpy(attri_keytype, "");
    attri_name.clear();
    attri_datatype.clear();

    value_num = 0;
    value.clear();
    value_datatype.clear();

    condition_num = 0;
    condi_attri_name.clear();
    condi_operator.clear();
    condi_data.clear();
    condi_datatype.clear();
}

/**
 * @description: just for testing, to see all info in this class
 * @param {*}
 * @return {*}
 */
void SQLCommand::print()
{
    vector<string>::iterator iter1, iter2, iter3, iter4;
    int i;

    cout << "filename: " << file_name << endl;
    cout << "tablename: " << table_name << endl;
    cout << "index_num: " << index_num << endl;
    cout << "index_name: " << index_name << endl;
    cout << "col_name: " << col_name << endl;

    cout << "attri_num: " << attri_num << endl;
    i = attri_num;
    iter1 = attri_name.begin();
    iter2 = attri_datatype.begin();
    while (i != 0)
    {
        cout << "attri_name: " << *iter1 << endl;
        cout << "attri_datatype: " << *iter2 << endl;

        iter1++;
        iter2++;
        i--;
    }
    cout << "attri_keytype: " << attri_keytype << endl;

    cout << "value_num: " << value_num << endl;
    i = value_num;
    iter1 = value.begin();
    iter2 = value_datatype.begin();
    while (i != 0)
    {
        cout << "value: " << *iter1 << endl;
        cout << "value_datatype: " << *iter2 << endl;

        iter1++;
        iter2++;
        i--;
    }

    cout << "condition_num: " << condition_num << endl;
    i = condition_num;
    iter1 = condi_attri_name.begin();
    iter2 = condi_operator.begin();
    iter3 = condi_data.begin();
    iter4 = condi_datatype.begin();
    while (i != 0)
    {
        cout << "condi_attri_name: " << *iter1 << endl;
        cout << "condi_operator: " << *iter2 << endl;
        cout << "condi_data: " << *iter3 << endl;
        cout << "condi_datatype: " << *iter4 << endl;

        iter1++;
        iter2++;
        iter3++;
        iter4++;
        i--;
    }

    cout << "---------------------------------------------------" << endl;
}
#endif
