/*
 * sqlquery.cc
 *
 *  Created on: Mar 11, 2017
 *      Author: ubuntu
 */

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "cJSON.h"
#include "sqlquery.h"

namespace search {

#define SUCCESS 0
#define PARSE_ERROR 1
#define BUFLENGTH_ERROR 2

#define DEST_CITY "select * from cityFtsTable WHERE cityFtsTable MATCH '%s';"
#define LOCAL_FULL "select count() from poiFtsTable WHERE poiFtsTable MATCH '\"%s\" AND %d';"


SqlQuery::SqlQuery() : sql_config_(NULL), sqlite_db_(NULL), json_response_(NULL) {
	// TODO Auto-generated constructor stub

}

SqlQuery::~SqlQuery() {
	// TODO Auto-generated destructor stub
	if (sql_config_) delete sql_config_;
	if (sqlite_db_) delete sqlite_db_;
	if (json_response_) delete[] json_response_;
	//city_map.clear();
}

bool SqlQuery::Init(ST_Config* config_tables, const char* db_path) {
	if (sql_config_) delete sql_config_;
	sql_config_ = new ST_Config;
	sql_config_->response_count = config_tables->response_count;
	if (sqlite_db_) delete sqlite_db_;
	sqlite_db_ = new SqliteDB();
	if (sqlite_db_->Open(db_path)) return true;
	//get all city
	/*Cursor* cursor = new Cursor();
	char str_sql[1024];
	memset(str_sql, '\0', sizeof(str_sql));
	sprintf(str_sql, "select * from cityFtsTable;");
	sqlite_db_->Query(str_sql, cursor);
	while (cursor->MoveToNext()) {
		city_map.insert(std::pair<std::string,int>(cursor->GetText(1), cursor->GetInt(0)));
	}
	delete cursor;*/
	return false;
}

void SqlQuery::GetDestCity(std::string& str_dest_city, int& city_code,
		std::vector<std::string>& vec_segment, std::string& cut_search, Cursor* cursor) {
	char str_sql[1024];
	unsigned int i = 0;
	for (i = 0; i < vec_segment.size(); ++i) {
		sprintf(str_sql, DEST_CITY, vec_segment[i].c_str());
		sqlite_db_->Query(str_sql, cursor);
		if (cursor->MoveToNext()) {
			city_code = cursor->GetInt(0);
			str_dest_city = cursor->GetText(1);
			cursor->Finalize();
			break;
		}
		cursor->Finalize();
	}
	cursor->Finalize();
	if (str_dest_city == "") return;
	for (unsigned int j = i+1; j < vec_segment.size(); ++j)
		cut_search += vec_segment[j];
}

void SqlQuery::GetWordSegment(std::vector<std::string>& vec_seg, const UnicodeString& query) {
	UErrorCode status = U_ZERO_ERROR;
	BreakIterator* bi = BreakIterator::createWordInstance(Locale::getUS(), status);
	bi->setText(query);
	int32_t p = bi->first();
	std::vector<int32_t> seg;

	while (p != BreakIterator::DONE) {
		seg.push_back(p);
		p = bi->next();
	}
	delete bi;

	for (unsigned int i = 0; i + 1 < seg.size(); ++i)
	{
		UnicodeString word;
		std::string utf8_word;
		query.extractBetween(seg[i], seg[i + 1], word);
		word.toUTF8String(utf8_word);
		vec_seg.push_back(utf8_word);
	}
}

char* SqlQuery::ProcessSearch(const char* json_buf) {
	memset(json_request_.match_info, '\0', sizeof(json_request_.match_info));
	if (SUCCESS != ParseJson(json_buf, &json_request_)) {
		return ResponseErrorJson(1, "json解析错误!");
	}
	Cursor* cursor = new Cursor();
	char str_sql[1024];
	// select count
	int localpart = 0, localfull = 0, overpart = 0, overfull = 0, dest_code = 0;
	//int limit_count = sql_config_->response_count;
	/*memset(str_sql, '\0', sizeof(str_sql));
	sprintf(str_sql, "select count() from poiFtsTable WHERE poiFtsTable " \
				"MATCH '%s AND %d';", json_request_.match_info, json_request_.city_code);
	sqlite_db_->Query(str_sql, cursor);
	if (cursor->MoveToNext())
		response_count = cursor->GetInt(0);
	sqlite_db_->Finalize(cursor->get_stmt());
	if (10 <= response_count && response_count < 1000) {
		memset(str_sql, '\0', sizeof(str_sql));
		sprintf(str_sql, "select poiName, addressName from poiFtsTable WHERE poiFtsTable " \
				"MATCH '%s AND %d' order by " \
				"user_rank(matchinfo(poiFtsTable, 'cls'), 0, 0, 0, 0, 15, 7, 0, 7, 0, starLevel) " \
				"DESC limit %d offset 0;", json_request_.match_info,
				json_request_.city_code, sql_config_->response_count);
	} else if (response_count >= 1000) {
		memset(str_sql, '\0', sizeof(str_sql));
		sprintf(str_sql, "select poiName, addressName from poiFtsTable WHERE poiFtsTable " \
				"MATCH '%s AND %d' " \
				"limit %d offset 0;", json_request_.match_info, json_request_.city_code, sql_config_->response_count);
	} else if (0 < response_count && response_count < 10) {
		memset(str_sql, '\0', sizeof(str_sql));
		sprintf(str_sql, "select count() from poiFtsTable WHERE poiFtsTable " \
				"MATCH '%s';", json_request_.match_info);
		if (cursor->MoveToNext())
			response_count = cursor->GetInt(0);
		sqlite_db_->Finalize(cursor->get_stmt());
		if (response_count < 1000)
			sprintf(str_sql, "select poiName, addressName from poiFtsTable WHERE poiFtsTable " \
					"MATCH '%s' order by " \
					"user_rank(matchinfo(poiFtsTable, 'cls'), 0, 0, 0, 0, 15, 7, 0, 7, 0, starLevel) " \
					"DESC limit %d offset 0;", json_request_.match_info, sql_config_->response_count);
		else
			sprintf(str_sql, "select poiName, addressName from poiFtsTable WHERE poiFtsTable " \
							"MATCH '%s' " \
							"limit %d offset 0;", json_request_.match_info, sql_config_->response_count);
	} else {
		delete cursor;
		return ResponseErrorJson(3, "无法搜索出结果!");
	}*/

	memset(str_sql, '\0', sizeof(str_sql));
	cJSON* root_json = CreateRootJson();
	cJSON* array_json = cJSON_CreateArray();
	//get city
	std::string str_dest_city = "", str_search = "", cut_search = "";
	std::vector<std::string> vec_segment;
	str_search = json_request_.match_info;
	//sprintf(str_sql, "select count() from poiFtsTable WHERE poiFtsTable " \
	//			"MATCH '\"%s\" AND %d';", json_request_.match_info, json_request_.city_code);
	sprintf(str_sql, LOCAL_FULL, json_request_.match_info, json_request_.city_code);
	sqlite_db_->Query(str_sql, cursor);
	if (cursor->MoveToNext())
			localfull = cursor->GetInt(0);
	cursor->Finalize();
	if (0 == localfull) {
		memset(str_sql, '\0', sizeof(str_sql));
		sprintf(str_sql, "select count() from poiFtsTable WHERE poiFtsTable " \
				"MATCH '\"%s\"';", json_request_.match_info);
		sqlite_db_->Query(str_sql, cursor);
		if (cursor->MoveToNext())
			overfull = cursor->GetInt(0);
		cursor->Finalize();
		//dest city
		//sprintf(str_sql, DEST_CITY, json_request_.match_info);
		//sqlite_db_->Query(str_sql, cursor);
		GetWordSegment(vec_segment, str_search.c_str());
		GetDestCity(str_dest_city, dest_code, vec_segment, cut_search, cursor);
		if (!str_dest_city.empty() && !cut_search.empty()) {
			memset(str_sql, '\0', sizeof(str_sql));
			sprintf(str_sql, "select count() from poiFtsTable WHERE poiFtsTable " \
					"MATCH '%s AND %d';", cut_search.c_str(), dest_code);
			sqlite_db_->Query(str_sql, cursor);
			if (cursor->MoveToNext())
				localpart = cursor->GetInt(0);
			cursor->Finalize();
			if (localpart > 0) {
				if (localpart < 1000) {
					memset(str_sql, '\0', sizeof(str_sql));
					sprintf(str_sql, "select poiName, addressName, adCode from poiFtsTable WHERE poiFtsTable " \
							"MATCH '%s AND %d' order by " \
							"user_rank(matchinfo(poiFtsTable, 'cls'), 0, 0, 0, 0, 15, 7, 0, 7, 0, starLevel) " \
							"DESC limit %d offset 0;", cut_search.c_str(),
							dest_code, sql_config_->response_count);
				} else if (localpart >= 1000) {
					memset(str_sql, '\0', sizeof(str_sql));
					sprintf(str_sql, "select poiName, addressName, adCode from poiFtsTable WHERE poiFtsTable " \
							"MATCH '%s AND %d' " \
							"limit %d offset 0;", cut_search.c_str(), dest_code, sql_config_->response_count);
				}
				sqlite_db_->Query(str_sql, cursor);
				ResponseJson(cursor, array_json);
				delete cursor;
				return PrintJson(root_json, array_json, localpart);
			}
		}
		// overall full search
		if (0 < overfull && overfull < 500) {
			memset(str_sql, '\0', sizeof(str_sql));
			sprintf(str_sql, "select poiName, addressName, adCode from poiFtsTable WHERE poiFtsTable " \
					"MATCH '\"%s\"' order by " \
					"user_rank(matchinfo(poiFtsTable, 'cls'), 0, 0, 0, 0, 15, 7, 0, 7, 0, starLevel) " \
					"DESC limit %d offset 0;", json_request_.match_info, sql_config_->response_count);
			sqlite_db_->Query(str_sql, cursor);
			ResponseJson(cursor, array_json);
			cursor->Finalize();
			delete cursor;
			return PrintJson(root_json, array_json, overfull);
		}
		/*if (overfull == 0) {
			// local part
			memset(str_sql, '\0', sizeof(str_sql));
			sprintf(str_sql, "select count() from poiFtsTable WHERE poiFtsTable " \
					"MATCH '%s AND %d';", json_request_.match_info, json_request_.city_code);
			sqlite_db_->Query(str_sql, cursor);
			if (cursor->MoveToNext())
				localpart = cursor->GetInt(0);
			cursor->Finalize();
			if (0 < localpart && localpart < 1000) {
				memset(str_sql, '\0', sizeof(str_sql));
				sprintf(str_sql, "select poiName, addressName from poiFtsTable WHERE poiFtsTable " \
						"MATCH '%s AND %d' order by " \
						"user_rank(matchinfo(poiFtsTable, 'cls'), 0, 0, 0, 0, 15, 7, 0, 7, 0, starLevel) " \
						"DESC limit %d offset 0;", json_request_.match_info,
						json_request_.city_code, sql_config_->response_count);
			} else if (localpart >= 1000) {
				memset(str_sql, '\0', sizeof(str_sql));
				sprintf(str_sql, "select poiName, addressName from poiFtsTable WHERE poiFtsTable " \
						"MATCH '%s AND %d' " \
						"limit %d offset 0;", json_request_.match_info, json_request_.city_code, sql_config_->response_count);
			}
			if (localpart > 0) {
				sqlite_db_->Query(str_sql, cursor);
				ResponseJson(cursor, array_json);
				delete cursor;
				return PrintJson(root_json, array_json, localpart);
			}
		}*/
	}
	//term/overall/dest+local
	/*
	 * 0.local part search, 0 < x < 1000
	 * 1.overall >= 500 && dest full match < 50,
	 * 2.overall == 0,
	 * 3. append, local full < limit, dest full < limit, overall full*/
	// local part
	memset(str_sql, '\0', sizeof(str_sql));
	sprintf(str_sql, "select count() from poiFtsTable WHERE poiFtsTable " \
			"MATCH '%s AND %d';", json_request_.match_info, json_request_.city_code);
	sqlite_db_->Query(str_sql, cursor);
	if (cursor->MoveToNext())
		localpart = cursor->GetInt(0);
	cursor->Finalize();
	if (0 < localpart && localpart < 1000) {
		memset(str_sql, '\0', sizeof(str_sql));
		sprintf(str_sql, "select poiName, addressName, adCode from poiFtsTable WHERE poiFtsTable " \
				"MATCH '%s AND %d' order by " \
				"user_rank(matchinfo(poiFtsTable, 'cls'), 0, 0, 0, 0, 15, 7, 0, 7, 0, starLevel) " \
				"DESC limit %d offset 0;", json_request_.match_info,
				json_request_.city_code, sql_config_->response_count);
	} else if (localpart >= 1000) {
		memset(str_sql, '\0', sizeof(str_sql));
		sprintf(str_sql, "select poiName, addressName, adCode from poiFtsTable WHERE poiFtsTable " \
				"MATCH '%s AND %d' " \
				"limit %d offset 0;", json_request_.match_info, json_request_.city_code, sql_config_->response_count);
	}
	if (localpart > 0) {
		sqlite_db_->Query(str_sql, cursor);
		ResponseJson(cursor, array_json);
		delete cursor;
		return PrintJson(root_json, array_json, localpart);
	}
	// overall part
	memset(str_sql, '\0', sizeof(str_sql));
	sprintf(str_sql, "select count() from poiFtsTable WHERE poiFtsTable " \
			"MATCH '%s';", json_request_.match_info);
	sqlite_db_->Query(str_sql, cursor);
	if (cursor->MoveToNext())
		overpart = cursor->GetInt(0);
	cursor->Finalize();
	// overall part search
	if (0 < overpart && overpart < 1000) {
		memset(str_sql, '\0', sizeof(str_sql));
		sprintf(str_sql, "select poiName, addressName, adCode from poiFtsTable WHERE poiFtsTable " \
				"MATCH '%s' order by " \
				"user_rank(matchinfo(poiFtsTable, 'cls'), 0, 0, 0, 0, 15, 7, 0, 7, 0, starLevel) " \
				"DESC limit %d offset 0;", json_request_.match_info, sql_config_->response_count);
	} else if (overpart >= 1000) {
		memset(str_sql, '\0', sizeof(str_sql));
		sprintf(str_sql, "select poiName, addressName, adCode from poiFtsTable WHERE poiFtsTable " \
				"MATCH '%s' " \
				"limit %d offset 0;", json_request_.match_info, sql_config_->response_count);
	}
	if (overpart > 0) {
		sqlite_db_->Query(str_sql, cursor);
		ResponseJson(cursor, array_json);
		delete cursor;
		return PrintJson(root_json, array_json, overpart);
	}

	delete cursor;
	return PrintJson(root_json, array_json, 0);
}

char* SqlQuery::ResponseErrorJson(int code, const char* msg) {
	if (json_response_) {
		delete[] json_response_;
		json_response_ = NULL;
	}
	cJSON * pJsonRoot = NULL;
	pJsonRoot = cJSON_CreateObject();
	if(NULL == pJsonRoot)
	{
		return NULL;
	}
	cJSON_AddNumberToObject(pJsonRoot, "code", 1);
	cJSON_AddStringToObject(pJsonRoot, "msg", msg);
	json_response_ = cJSON_Print(pJsonRoot);
	if(NULL == json_response_)
	{
		cJSON_Delete(pJsonRoot);
		return NULL;
	}
	cJSON_Delete(pJsonRoot);
	return json_response_;
}

cJSON* SqlQuery::CreateRootJson() {
	cJSON * pJsonRoot = NULL;
	pJsonRoot = cJSON_CreateObject();
	if(NULL == pJsonRoot)
	{
		//error happend here
		return NULL;
	}
	cJSON_AddNumberToObject(pJsonRoot, "code", 0);
	cJSON_AddStringToObject(pJsonRoot, "msg", "");
	return pJsonRoot;
}

char* SqlQuery::PrintJson(cJSON* root_json, cJSON* array_json, int result_count) {
	if (json_response_) {
		delete[] json_response_;
		json_response_ = NULL;
	}
	cJSON_AddNumberToObject(root_json, "responseCount", result_count);
	cJSON_AddItemToObject(root_json, "responseJson", array_json);

	json_response_ = cJSON_Print(root_json);
	// else use :
	// char * p = cJSON_PrintUnformatted(pJsonRoot);
	if(NULL == json_response_)
	{
		cJSON_Delete(root_json);
		return NULL;
	}
	cJSON_Delete(root_json);
	return json_response_;
}

void SqlQuery::ResponseJson(Cursor* cursor, cJSON* array_json) {
	while(cursor->MoveToNext()) {
		cJSON* pItem = cJSON_CreateObject();
		const char* poi_name = cursor->GetText(0);
		const char* poi_address = cursor->GetText(1);
		int adCode = cursor->GetInt(2);
		cJSON_AddStringToObject(pItem, "poiName", poi_name);
		cJSON_AddStringToObject(pItem, "addressName", poi_address);
		cJSON_AddNumberToObject(pItem, "adCode", adCode);
		cJSON_AddItemToArray(array_json, pItem);
	}
}

int SqlQuery::ParseJson(const char* json_buf, ST_JsonRequest* json_request) {
	if(NULL == json_buf)
		return PARSE_ERROR;
	cJSON * pJson = cJSON_Parse(json_buf);
	if(NULL == pJson)
		return PARSE_ERROR;

	// get match info from json
	cJSON * pSub = cJSON_GetObjectItem(pJson, "match");
	if(NULL == pSub)
	{
		cJSON_Delete(pJson);
		return PARSE_ERROR;
	}
	if (strlen(pSub->valuestring) >= sizeof(json_request->match_info)) {
		cJSON_Delete(pJson);
		return BUFLENGTH_ERROR;
	}
	if (strlen(pSub->valuestring) == 0) {
		cJSON_Delete(pJson);
		return BUFLENGTH_ERROR;
	}
	memcpy(json_request->match_info, pSub->valuestring, strlen(pSub->valuestring));

	// get city from json
	pSub = cJSON_GetObjectItem(pJson, "city");
	if(NULL == pSub)
	{
		cJSON_Delete(pJson);
		return PARSE_ERROR;
	}
	json_request->city_code = pSub->valueint;

	cJSON_Delete(pJson);
	return SUCCESS;
}

} /* namespace search */

