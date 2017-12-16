#ifndef SQLITE_DATA_BASE_H
#define SQLITE_DATA_BASE_H
#include "sqlite3.h"
#include <string>
#include "cursor.h"
#include <vector>
#include <stdint.h>

namespace search {


class SqliteDB {
public:
	SqliteDB();
	SqliteDB(const SqliteDB& db);
	SqliteDB& operator=(const SqliteDB& db);
	~SqliteDB();

public:
	/**
	*  @brief open database connect
	*  path: database file
	*/
	bool Open(const char* path);
	/**
	*  @brief create database
	*  path: database file
	*/
	bool CreateDB(const std::string& path);
	/**
	* @brief exec sql, not use for query
	**/
	bool ExecSql(const std::string& sql);

	/**
	* @brief query action.
	**/
	bool Query(const char* sql, Cursor* cursor);

	bool Query(const std::string& sql, const std::vector<std::string>& params, Cursor* cursor);


	/**
	*@brief insert sql
	*/
	bool insert(const std::string& sql);

	bool insert(const std::string& sql, const std::vector<std::string>& params);

	sqlite3_stmt * prepare(const std::string& sql);

	void bind_int(sqlite3_stmt* stmt, int index, int value){
			sqlite3_bind_int(stmt, index, value);
	}

	void bind_int64(sqlite3_stmt* stmt, int index, int64_t value){
			sqlite3_bind_int64(stmt, index, value);
	}

	void bind_blob(sqlite3_stmt* stmt, int index, char* value,int length){
			sqlite3_bind_blob(stmt, index, value, length, NULL);
	}

	void bind_text(sqlite3_stmt* stmt, int index, char* value){
			sqlite3_bind_text(stmt, index, value, -1, NULL);
	}

	bool Finalize(sqlite3_stmt *);
private:
	sqlite3* db;
};

} //namespace search
#endif
