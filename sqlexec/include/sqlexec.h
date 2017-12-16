/*
 * sqlexec.h
 *
 *  Created on: 03/11/2017
 *      Author: wuchunlei
 */

#ifndef SQLEXEC_H_
#define SQLEXEC_H_

#include "sqlexec_define.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagsqlexec_t sqlexec_t;

extern sqlexec_t* InitSqlExec(ST_Config* config_tables, const char* db_path);

extern char* ProcessSqlExec(sqlexec_t* ptr_sqlexec, const char* json_buf);

extern void CloseSqlExec(sqlexec_t* ptr_sqlexec);


#ifdef __cplusplus
}   /* end extern "C" */
#endif

#endif /* SQLEXEC_H_ */
