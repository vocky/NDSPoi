#include "stdio.h"
#include "sqlitedb.h"
#include <string>

namespace search {

SqliteDB::SqliteDB():db(NULL) {

}

SqliteDB::~SqliteDB() {
	if (db)
		sqlite3_close(db);
}

bool SqliteDB::Open(const char* path) {
	int rc = sqlite3_open(path, &db);
	if (rc){
		printf("SqliteDataBase::Open error, msg:%s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		db = NULL;
		return false;
	}
	return true;
}

bool SqliteDB::CreateDB(const std::string& path) {
	if (!Open(path.c_str())) return false;
	//std::string str_poi = "CREATE VIRTUAL TABLE if not exists poiFtsTable USING fts4(poiName, adress, tokenize=icu zh_CN)";
	std::string str_poi = "CREATE VIRTUAL TABLE if not exists poiFtsTable USING fts4" \
			"(poiId INTEGER NOT NULL, adCode INTEGER NOT NULL, mortonCode INTEGER, " \
			"naviMorton INTEGER, poiName TEXT NOT NULL, " \
             "addressName TEXT, " \
			 "poiCategoryId INTEGER NOT NULL, categoryName TEXT NOT NULL, starLevel INTEGER NOT NULL, " \
			 "tokenize=icu zh_CN, notindexed='poiId', notindexed='adCode', notindexed='mortonCode', " \
			 "notindexed='naviMorton', notindexed='poiCategoryId', notindexed='starLevel');";
	if (!ExecSql(str_poi)) return false;
	return true;
}

bool SqliteDB::ExecSql(const std::string& sql){
	if (!db){
		printf("SqliteDataBase::Open dataset error.\n");
		return false;
	}

	char *p = NULL;
	int rc = sqlite3_exec(db, sql.c_str(), NULL, NULL, &p);
	if (rc && p){
		printf("SqliteDataBase::exec error, msg:%s\n", p);
		return false;
	}
	return true;
}

bool SqliteDB::Query(const char* sql, Cursor* cursor){
	if (!db){
		printf("SqliteDataBase::Open dataset error.\n");
		return false;
	}
	sqlite3_stmt * stmt;
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	cursor->set_stmt(stmt);
	return true;
}
 bool SqliteDB::Query(const std::string& sql, const std::vector<std::string>& params, Cursor* cursor){
	if (!db){
		printf("SqliteDataBase::Open dataset error.\n");
		return false;
	}
	sqlite3_stmt * stmt;
	sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, NULL);
	for (int i = 0; i < params.size(); ++i){
		sqlite3_bind_text(stmt, i + 1, params[i].c_str(), sql.length(), SQLITE_STATIC);
	}
	cursor->set_stmt(stmt);
	return true;
}

bool SqliteDB::insert(const std::string& sql){
	if (!db){
		printf("SqliteDataBase::Open dataset error.\n");
		return false;
	}
	sqlite3_stmt * stmt;
	sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, NULL);
	int rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE){
		sprintf("SqliteDataBase::insert data error ,%s \n", sqlite3_errmsg(db));
	}
	sqlite3_finalize(stmt);
	return true;
}

bool SqliteDB::insert(const std::string& sql, const std::vector<std::string>& params){
	if (!db){
		printf("SqliteDataBase::Open dataset error.\n");
		return false;
	}
	sqlite3_stmt * stmt;
	sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, NULL);
	for (int i = 0; i < params.size(); ++i){
		sqlite3_bind_text(stmt, i + 1, params[i].c_str(), sql.length(), SQLITE_STATIC);
	}
	int rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE){
		sprintf("SqliteDataBase::insert data error ,%s \n", sqlite3_errmsg(db));
	}
	sqlite3_finalize(stmt);
	return true;
	}

sqlite3_stmt* SqliteDB::prepare(const std::string& sql){
	if (!db){
		printf("SqliteDataBase::Open dataset error.\n");
		return NULL;
	}
	sqlite3_stmt * stmt;
	sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, NULL);
	return stmt;
}

bool SqliteDB::Finalize(sqlite3_stmt* stmt){
	sqlite3_finalize(stmt);
	return true;
}
} //namespace search
