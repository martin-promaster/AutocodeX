#include "stdafx.h"

#include "platform.h"

int GetDatabaseContextFromRegistry(EX_MYSQL_CONTEXT &mysql_ctx)
{
	//EX_MYSQL_CONTEXT mysql_ctx;

	HKEY hKey;
	DWORD cbData = 0;
	LSTATUS lResult = RegOpenKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\autocodeX\\DatabaseContext", REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, &hKey);
	if (lResult != ERROR_SUCCESS)
	{
		if (lResult == ERROR_FILE_NOT_FOUND) {
			RegCreateKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\autocodeX\\DatabaseContext", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);
			RegSetValueEx(hKey, L"DBIpAddress", 0, RRF_RT_REG_SZ, NULL, cbData);
			RegSetValueEx(hKey, L"DBSchemaName", 0, RRF_RT_REG_SZ, NULL, cbData);
			RegSetValueEx(hKey, L"DBUser", 0, RRF_RT_REG_SZ, NULL, cbData);
			RegSetValueEx(hKey, L"DBPassword", 0, RRF_RT_REG_SZ, NULL, cbData);
			RegSetValueEx(hKey, L"DBTableName", 0, RRF_RT_REG_SZ, NULL, cbData);
			//printf("Key not found.\n");
			//return TRUE;
		}
		else {
			//printf("Error opening key.\n");
			//return FALSE;
		}
	}

	// Determin the length of the string.
	RegGetValue(hKey, NULL, L"DBUser", RRF_RT_REG_SZ, NULL, NULL, &cbData);
	mysql_ctx.dbuser = new TCHAR[cbData];
	RegGetValue(hKey, NULL, L"DBUser", RRF_RT_REG_SZ, NULL, mysql_ctx.dbuser, &cbData);

	RegGetValue(hKey, NULL, L"DBPassword", RRF_RT_REG_SZ, NULL, NULL, &cbData);
	mysql_ctx.dbpasswd = new TCHAR[cbData];
	RegGetValue(hKey, NULL, L"DBPassword", RRF_RT_REG_SZ, NULL, mysql_ctx.dbpasswd, &cbData);

	RegGetValue(hKey, NULL, L"DBIpAddress", RRF_RT_REG_SZ, NULL, NULL, &cbData);
	mysql_ctx.dbip = new TCHAR[cbData];
	RegGetValue(hKey, NULL, L"DBIpAddress", RRF_RT_REG_SZ, NULL, mysql_ctx.dbip, &cbData);

	RegGetValue(hKey, NULL, L"DBSchemaName", RRF_RT_REG_SZ, NULL, NULL, &cbData);
	mysql_ctx.dbname = new TCHAR[cbData];
	RegGetValue(hKey, NULL, L"DBSchemaName", RRF_RT_REG_SZ, NULL, mysql_ctx.dbname, &cbData);

	RegGetValue(hKey, NULL, L"DBTableName", RRF_RT_REG_SZ, NULL, NULL, &cbData);
	mysql_ctx.tablename = new TCHAR[cbData];
	RegGetValue(hKey, NULL, L"DBTableName", RRF_RT_REG_SZ, NULL, mysql_ctx.tablename, &cbData);

	mysql_ctx.dbport = 6033;

	return RegCloseKey(hKey);
}