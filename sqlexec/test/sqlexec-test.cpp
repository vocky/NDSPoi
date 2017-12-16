//============================================================================
// Name        : sqlexec-test.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "cJSON.h"
#include "sqlexec_define.h"
#include "sqlexec.h"

using namespace std;

char* makeJson()
{
    cJSON * pJsonRoot = NULL;

    pJsonRoot = cJSON_CreateObject();
    if(NULL == pJsonRoot)
    {
        //error happend here
        return NULL;
    }
    cJSON_AddStringToObject(pJsonRoot, "match", "上海路");
    cJSON_AddNumberToObject(pJsonRoot, "city", 310000);
    /*cJSON_AddBoolToObject(pJsonRoot, "bool", 1);
    cJSON * pSubJson = NULL;
    pSubJson = cJSON_CreateObject();
    if(NULL == pSubJson)
    {
        // create object faild, exit
        cJSON_Delete(pJsonRoot);
        return NULL;
    }
    cJSON_AddStringToObject(pSubJson, "subjsonobj", "a sub json string");
    cJSON_AddItemToObject(pJsonRoot, "subobj", pSubJson);*/

    char * p = cJSON_Print(pJsonRoot);
  // else use :
    // char * p = cJSON_PrintUnformatted(pJsonRoot);
    if(NULL == p)
    {
        cJSON_Delete(pJsonRoot);
        return NULL;
    }
    //free(p);

    cJSON_Delete(pJsonRoot);

    return p;
}

int main() {
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	ST_Config st_config;
	st_config.response_count = 20;
	sqlexec_t* sql_exec = InitSqlExec(&st_config, "/home/ubuntu/Documents/poi_search/FTS5.NDS");
	char* json_buf = makeJson();
	char* result = ProcessSqlExec(sql_exec, json_buf);
	cout << result << endl;
	delete[] json_buf;
	CloseSqlExec(sql_exec);
	cout << "!!!success!!!" << endl; // prints !!!Hello World!!!
	return 0;
}
