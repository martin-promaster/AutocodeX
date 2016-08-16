#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")

#include <mysql.h>
#pragma comment(lib, "libmysql.lib")

/*
  Added by: Martin Dong
  Date: 2015-04-09
  Description: This struct used to create java get/set object file.
  -----------------------------------------------------------------------
  The Java get/set method stype is listed below:
  
	  public <field_type> get<method_name>() {
		return <field_name>;
	  }
  
	  public void set<method_name>(<field_type> p0) {
		<field_name> = p0;
	  }
*/
typedef struct java_pojo_object
{
	char* file_name;

	char* field_name;
	char* field_type;

	char* get_method_name;
	char* set_method_name;
} J_POJO_OBJ;

typedef struct tMySQLContext {
	TCHAR *dbuser;
	TCHAR *dbpasswd;
	TCHAR *dbip;
	unsigned int dbport = 3306;
	TCHAR *dbname;
	TCHAR *tablename;
} EX_MYSQL_CONTEXT;

char* plt_get_mysql_field_type_name(enum_field_types field_type);

TCHAR* plt_MultiByteToWideChar(LPCCH lpMutiByteStr);
LPCH plt_WideCharToMultiByte(TCHAR* lpWideCharStr);

char* plt_toChar(char* lpszStr, size_t strLen);

void mysql_table2code(HWND hWnd);