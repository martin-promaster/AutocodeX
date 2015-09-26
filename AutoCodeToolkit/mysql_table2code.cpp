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
	char dbuser[30] = "redmine";
	char dbpasswd[30] = "123456";
	char dbip[30] = "172.16.161.211";
	unsigned int dbport = 3306;
	char dbname[50] = "redmine";
	char tablename[50] = "issues";
	char *query = NULL;

	if ( NULL == mysql_real_connect(myConn, dbip, dbuser, dbpasswd, 
		dbname,	dbport, NULL, 0) )
	{
		const char *errMessage = NULL;
		errMessage = mysql_error(myConn);
		MessageBox(0, L"Connect to database error", L"CAUTION", MB_OKCANCEL);
		return;
	}

	if ( NULL != mysql_select_db(myConn, dbname))
	{
		const char *errMessage = NULL;
		errMessage = mysql_error(myConn);
		MessageBox(0, L"select database error", L"CAUTION", MB_OKCANCEL);
		return;
	}

	char tmpSql[640];
	int ret;

	sprintf_s(tmpSql, "select * from %s", tablename);

	if (mysql_real_query(myConn, tmpSql, strlen(tmpSql)))
	{
		printf("Error making query: %s !!!\n", mysql_error(myConn));
	}


	// 将结果保存在 MYSQL_RES 结构体中
	MYSQL_RES* resultset = mysql_store_result(myConn);

	// Prepare fille handle to store the result set.
	// +
	DWORD dwBytesRead, dwBytesWritten, dwPos;
	HANDLE hfile = CreateFile(L"test.java", GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
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

