#ifndef _INTERFACE_
#define _INTERFACE_

#include "catalog.h"
#include "record.h"
#include "SQLCommand.hpp"
#include <map>
#include <cstring>
#include "BufferManager.h"
#include "init.h"
#include "initBlock.h"

//interact with API
void interface(SQLCommand &com);

#endif