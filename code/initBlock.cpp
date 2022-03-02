// #include "initBlock.h"

// tmpTable momTable;
// BufferManager buffer;

// //create the init block, return its index
// int createInitBlock()
// {
//     int index;
//     int tupleSize;
//     tupleSize = 2 * sizeof(int);
//     char *ptr;
//     index = createTupleBlock(tupleSize, ptr);
//     return index;
// }

// //extract the information in the block, store them in the map
// void initMap(int index)
// {
//     tableMap.clear();
//     attrVec columns;
//     columns.push_back(attribute("catalog", INT_TPYE, 0, 0, 0, 0, "0"));
//     columns.push_back(attribute("pRecord", INT_TPYE, 0, 0, 0, 0, "0"));
//     momTable.extract(index);
//     for (auto &row : momTable.tupleVec)
//     {
//         myTable tmp;
//         tmp = myTable(*(int *)row[0], *(int *)row[1]);
//         tableMap[string(tmp.catalog.name)] = tmp;
//     }
// }