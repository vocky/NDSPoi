/*
 * sqlexec.cc
 *
 *  Created on: 03/11/2017
 *      Author: wuchunlei
 */

#include "sqlexec.h"

#include <stdlib.h>
#include <unistd.h>

#include "sqlquery.h"

using search::SqlQuery;

extern "C" {

    struct tagsqlexec_t {SqlQuery* rep;};

    sqlexec_t* InitSqlExec(ST_Config* config_tables, const char* db_path) {
    	SqlQuery* sql_query = new SqlQuery();
        bool binit = sql_query->Init(config_tables, db_path);
        if (!binit)
            return NULL;
        sqlexec_t* result = new sqlexec_t;
        result->rep = sql_query;
        return result;
    }

    char* ProcessSqlExec(sqlexec_t* ptr_sqlexec, const char* json_buf) {
        if (!ptr_sqlexec) return NULL;
        return ptr_sqlexec->rep->ProcessSearch(json_buf);
    }

    void CloseSqlExec(sqlexec_t* ptr_sqlexec) {
        if (ptr_sqlexec) {
            delete ptr_sqlexec->rep;
            delete ptr_sqlexec;
        }
    }
}   //end extern "C"
