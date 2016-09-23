#include "stdafx.h"

#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>

#include "mysql_table2code.h"



char* make_java_method_name(char* name, const char* method_prefix)
{
	int nLen = 3 + strlen(name) + 1;
	char* pRet = new char[nLen];
	memset(pRet, 0, nLen);

	strcpy_s(pRet, 4, method_prefix);
	strcat_s(pRet, nLen, name);

	pRet[3] = pRet[3] - 32;

	return pRet;

}

char* make_java_field_name(char *name)
{
	char* pRet = new char[strlen(name)+1];
	memset(pRet, 0, strlen(name) + 1);

	char *q = pRet;

	char* p = name;
	while (*p)
	{
		if (*p == '_')
		{
			p++;
			*p = *p - 32;
		}
		*q++ = *p++;
	}

	return pRet;
}


void mysql_table2code(HWND hWnd)
{
	// Get display device.
	HDC hdc = GetDC(hWnd);


	MYSQL* myConn = mysql_init(NULL);

	if (NULL == myConn)
	{
		MessageBox(0, L"", L"", MB_OKCANCEL);
		return;
	}

	//database configuartion
	/*
	HKEY key;
	RegCreateKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\autocodeX\\DatabaseContext", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, NULL);
	RegSetKeyValue(key, NULL, L"DBUser", REG_SZ, L"iems_user", sizeof("iems_user")*2);
	RegSetKeyValue(key, NULL, L"DBPassword", REG_SZ, L"iems$2016", sizeof("iems$2016") * 2);
	RegSetKeyValue(key, NULL, L"DBIpAddress", REG_SZ, L"10.129.132.224", sizeof("10.129.132.224") * 2);
	RegSetKeyValue(key, NULL, L"DBSchemaName", REG_SZ, L"iems_db", sizeof("iems_db") * 2);
	RegSetKeyValue(key, NULL, L"DBTableName", REG_SZ, L"tbl_sys_oper", sizeof("tbl_sys_oper") * 2);
	RegCloseKey(key);
	*/

	EX_MYSQL_CONTEXT mysql_ctx;

	GetDatabaseContextFromRegistry(mysql_ctx);

	char *query = NULL;

	
	if ( NULL == mysql_real_connect(myConn, 
		plt_WideCharToMultiByte(mysql_ctx.dbip),
		plt_WideCharToMultiByte(mysql_ctx.dbuser),
		plt_WideCharToMultiByte(mysql_ctx.dbpasswd),
		plt_WideCharToMultiByte(mysql_ctx.dbname),
		mysql_ctx.dbport, NULL, 0) )
	{
		const char *errMessage = NULL;
		errMessage = mysql_error(myConn);
		MessageBox(0, L"Connect to database error", L"CAUTION", MB_OKCANCEL);
		return;
	}

	if ( NULL != mysql_select_db(myConn, plt_WideCharToMultiByte(mysql_ctx.dbname) ))
	{
		const char *errMessage = NULL;
		errMessage = mysql_error(myConn);
		MessageBox(0, L"select database error", L"CAUTION", MB_OKCANCEL);
		return;
	}

	TCHAR tmpSql[640];
	int ret;

	wsprintf(tmpSql, L"select * from %s", mysql_ctx.tablename);

	char *sSql = plt_WideCharToMultiByte(tmpSql);
	if (mysql_real_query(myConn, sSql, strlen(sSql)))
	{
		TCHAR* errMsg = new TCHAR[1024];
		memset(errMsg, 0, lstrlen(errMsg));
		wsprintf(errMsg,  L"Error making query: %s !!!\n", plt_MultiByteToWideChar(mysql_error(myConn)));
		MessageBox(0, errMsg, L"", MB_OKCANCEL);
		return ;
	}


	// 将结果保存在 MYSQL_RES 结构体中
	MYSQL_RES* resultset = mysql_store_result(myConn);

	// Prepare fille handle to store the result set.
	// +
	TCHAR *strOutputFilename = new TCHAR[128];
	lstrcpy(strOutputFilename, mysql_ctx.tablename);
	lstrcat(strOutputFilename, L".java");

	DWORD dwBytesRead, dwBytesWritten, dwPos;

	HANDLE hfile = CreateFile(strOutputFilename, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hfile == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hfile);
		return;
	}

	// 
	char szJavaClassBegin[] = "public class test {\n";
	
	char szJavaClassEnd[] = "\n}";
	WriteFile(hfile, szJavaClassBegin, strlen(szJavaClassBegin), &dwBytesWritten, NULL);
	// -

	int nPosX = 10;
	int nPosY = 50;

	MYSQL_FIELD *field;
	while (field = mysql_fetch_field(resultset))
	{
		J_POJO_OBJ pojo;

		pojo.field_name = make_java_field_name(field->org_name);;
		pojo.field_type = plt_get_mysql_field_type_name(field->type);
		pojo.get_method_name = make_java_method_name(pojo.field_name, "get");
		pojo.set_method_name = make_java_method_name(pojo.field_name, "set");

		// Generate the POJO class file.
		char output[1024];

		sprintf_s(output, "public %s %s;", pojo.field_type, pojo.field_name);
		// Calculate the display position.
		// +
		TCHAR *pRet = plt_MultiByteToWideChar(output);
		int nRet = lstrlen(pRet);
		TextOut(hdc, nPosX, nPosY, pRet, nRet);
		SIZE size;
		GetTextExtentPoint(hdc, pRet, nRet, &size);
		nPosY += size.cy;
		// -
		WriteFile(hfile, output, nRet, &dwBytesWritten, NULL);



		sprintf_s(output, "public %s %s () {", pojo.field_type, pojo.get_method_name);
		// Calculate the display position.
		// +
		pRet = plt_MultiByteToWideChar(output);
		nRet = lstrlen(pRet);
		TextOut(hdc, nPosX, nPosY, pRet, nRet);
		//SIZE size;
		GetTextExtentPoint(hdc, pRet, nRet, &size);
		nPosY += size.cy;
		// -
		WriteFile(hfile, output, nRet, &dwBytesWritten, NULL);



		sprintf_s(output, "    return %s; }", pojo.field_name);
		// Calculate the display position.
		// +
		pRet = plt_MultiByteToWideChar(output);
		nRet = lstrlen(pRet);
		TextOut(hdc, nPosX, nPosY, pRet, nRet);
		//SIZE size;
		GetTextExtentPoint(hdc, pRet, nRet, &size);
		nPosY += size.cy;
		// -
		WriteFile(hfile, output, nRet, &dwBytesWritten, NULL);



		sprintf_s(output, "public void %s (%s p0) {", pojo.set_method_name, pojo.field_type);
		// Calculate the display position.
		// +
		pRet = plt_MultiByteToWideChar(output);
		nRet = lstrlen(pRet);
		TextOut(hdc, nPosX, nPosY, pRet, nRet);
		//SIZE size;
		GetTextExtentPoint(hdc, pRet, nRet, &size);
		nPosY += size.cy;
		// -
		WriteFile(hfile, output, nRet, &dwBytesWritten, NULL);



		sprintf_s(output, "    %s = p0; }", pojo.field_name);
		// Calculate the display position.
		// +
		pRet = plt_MultiByteToWideChar(output);
		nRet = lstrlen(pRet);
		TextOut(hdc, nPosX, nPosY, pRet, nRet);
		//SIZE size;
		GetTextExtentPoint(hdc, pRet, nRet, &size);
		nPosY += size.cy + 20;
		// -
		WriteFile(hfile, output, nRet, &dwBytesWritten, NULL);

		WriteFile(hfile, L"\n", 1, &dwBytesWritten, NULL);

		printf("");
	}

	WriteFile(hfile, szJavaClassEnd, strlen(szJavaClassEnd), &dwBytesWritten, NULL);

	CloseHandle(hfile);

	/*
	MYSQL_ROW row;
	int rowIndex = 0;
	int colIndex = 1;
	while (row = mysql_fetch_row(resultset))
	{
		rowIndex = 10;
		for (int t = 0; t < mysql_num_fields(resultset); t++)
		{
			char tmp[1024];
			sprintf_s(tmp, "%s", row[t]);

			TCHAR *pRet = plt_MultiByteToWideChar(tmp);
			int nRet = lstrlen(pRet);
			
			// Calculate the display position.
			// +
			TextOut(hdc, rowIndex, 50 * colIndex, pRet, nRet);
			SIZE size;
			GetTextExtentPoint(hdc, pRet, nRet, &size);
			rowIndex += size.cx + 30;
			// -
		}
		colIndex++;
	}
	*/

	mysql_close(myConn);
}


TCHAR* plt_MultiByteToWideChar(LPCCH lpMutiByteStr)
{
	int nSize = strlen(lpMutiByteStr);

	int nLen = MultiByteToWideChar(CP_ACP, 0, lpMutiByteStr, -1, NULL,NULL);

	TCHAR *lpWideCharStr = new TCHAR[nLen];
	memset(lpWideCharStr, 0, nLen*2);

	MultiByteToWideChar(CP_ACP, 0, lpMutiByteStr, nSize, lpWideCharStr, nLen);

	return lpWideCharStr;

}

LPCH plt_WideCharToMultiByte(TCHAR* lpWideCharStr)
{
	int nSize = lstrlen(lpWideCharStr);

	int nLen = WideCharToMultiByte(CP_OEMCP, 0, lpWideCharStr, -1, NULL, 0, NULL, FALSE);

	LPCH lpMutiByteStr = new CHAR[nLen];
	memset(lpMutiByteStr, 0, nLen);

	WideCharToMultiByte(CP_OEMCP, 0, lpWideCharStr, nSize, lpMutiByteStr, nLen, NULL, FALSE);

	return lpMutiByteStr;

}


char* plt_get_mysql_field_type_name(enum_field_types field_type)
{
	switch (field_type)
	{
	case 0:	return "Decimal";   //MYSQL_TYPE_DECIMAL
	case 1:	return "int";       //MYSQL_TYPE_TINY
	case 2:	return "Short";     //MYSQL_TYPE_SHORT
	case 3:	return "Long";      //MYSQL_TYPE_LONG
	case 4:	return "Float";     //MYSQL_TYPE_FLOAT
	case 5:	return "Double";    //MYSQL_TYPE_DOUBLE
	case 6:	return "NULL";      //MYSQL_TYPE_NULL
	case 7:	return "Timestamp"; //MYSQL_TYPE_TIMESTAMP
	case 12: return "Date";     //MYSQL_TYPE_DATETIME
	case 253: return "String";  //MYSQL_TYPE_VAR_STRING
	default: return "";
	}
}

