#ifndef CURSOR_H
#define CURSOR_H

#include <stdint.h>
#include "sqlite3.h"
#include <string.h>

namespace search {

class Cursor {
public:
	Cursor() {
		stmt_ = NULL;
	}

	~Cursor() {
		if (stmt_) sqlite3_finalize(stmt_);;
	}

public:
	void set_stmt(sqlite3_stmt* stmt) {
		stmt_ = stmt;
	}

	sqlite3_stmt* get_stmt() {return stmt_;}
	/**
	*  @brief Get Colun index by colun nae
	*   return colun index
	**/
	int GetColunIndex(const char* name)const {
		int count = sqlite3_column_count(stmt_);
		for (int i = 0; i < count; ++i){
			if (strcmp(name, sqlite3_column_name(stmt_, i)) == 0){
					return i;
			}
		}
		return -1;
	}
	/**
	*  @brief get value by colun index in current colun data set
	**/
	int GetInt(int colun_index)const{
		return sqlite3_column_int(stmt_, colun_index);
	}

	int64_t GetInt64(int colun_index) const{
		return sqlite3_column_int64(stmt_, colun_index);
	}
	/**
	*       @brief get blob by colun index at current row
	*  size: set size
	**/
	const void* GetBlob(int colun_index,unsigned int *size)const {
		*size = sqlite3_column_bytes(stmt_, colun_index);
		return sqlite3_column_blob(stmt_,colun_index);
	}

	/**
	*       @brief get blob by colun index at current row
	*  size: set size
	**/
	const char* GetText(int colun_index)const {
		//*size = sqlite3_column_bytes(stmt_, colun_index);
		return (const char*)sqlite3_column_text(stmt_, colun_index);
	}


	/**
	* @brief get count of colun
	**/
	int GetColunCount() const {
		return sqlite3_column_count(stmt_);
	}

	bool MoveToNext(){
		int code = sqlite3_step(stmt_);
		if (code == SQLITE_ROW){
			return true;
		}
		else{
			return false;
		}
	}
	void Finalize() {
		if (stmt_) sqlite3_finalize(stmt_);
		stmt_ = NULL;
	}

private:
	sqlite3_stmt* stmt_;
};

} //namespace search

#endif
