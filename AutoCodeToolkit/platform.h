#pragma once

// User defined functions.

typedef struct tMySQLContext {
	TCHAR *dbuser;
	TCHAR *dbpasswd;
	TCHAR *dbip;
	unsigned int dbport = 3306;
	TCHAR *dbname;
	TCHAR *tablename;
} EX_MYSQL_CONTEXT;


int GetDatabaseContextFromRegistry(EX_MYSQL_CONTEXT &mysql_ctx);