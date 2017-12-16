/*
 * sqlquery.h
 *
 *  Created on: Mar 11, 2017
 *      Author: ubuntu
 */

#ifndef SQLEXEC_SQLQUERY_H_
#define SQLEXEC_SQLQUERY_H_

#include "unicode/ubrk.h"
#include "unicode/unistr.h"
#include "unicode/brkiter.h"
#include <string>
#include <vector>
#include "sqlexec_define.h"
#include "sqlitedb.h"
#include "cJSON.h"

namespace search {

class SqlQuery {
public:
        SqlQuery();
        virtual ~SqlQuery();
        bool Init(ST_Config* config_tables, const char* db_path);
        char* ProcessSearch(const char* json_buf);
private:
        int ParseJson(const char* json_buf, ST_JsonRequest* json_request);
        void ResponseJson(Cursor* cursor, cJSON* array_json);
        char* ResponseErrorJson(int code, const char* msg);
        cJSON* CreateRootJson();
        char* PrintJson(cJSON* root_json, cJSON* array_json, int result_count);
        //get dest city
        void GetDestCity(std::string& str_dest_city, int& city_code,
        		std::vector<std::string>& vec_segment, std::string& cut_search, Cursor* cursor);
        void GetWordSegment(std::vector<std::string>& vec_seg, const UnicodeString& query);
        ST_Config* sql_config_;
        SqliteDB* sqlite_db_;
        ST_JsonRequest json_request_;
        char* json_response_;
        //std::map<std::string, int> city_map;
};

} /* namespace search */

#endif /* SQLEXEC_SQLQUERY_H_ */
