#include "sqlite3.h"
#include<stdio.h>
#include<stdlib.h>

static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    int i;
    for (int i=0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");  
    }
    printf("\n");
    return 0;
}

int main(int argc, char const *argv[])
{

	sqlite3 *db, *db2;
	char *errmsg;
	char* dbName = "aes_cbc_128.db";
    char *sql=NULL;


	if(SQLITE_OK != sqlite3_open(":memory:", &db2)){
		printf("sqlite3_open error\n");
        exit(1);
		sqlite3_close(db);
	}

	if(SQLITE_OK != sqlite3_enable_load_extension(db2, 1)){
		printf("load error");
	}
	
	if(SQLITE_OK != sqlite3_load_extension(db2, "./cksumvfs", 0, &errmsg)){
		printf("error1 : %s\n", errmsg);
	  	sqlite3_free(errmsg);
	}

    // sqlite3_close(db2);


	if(SQLITE_OK != sqlite3_open(dbName, &db)){
		printf("sqlite3_open error\n");
        exit(1);
		sqlite3_close(db);
	}

	// if(SQLITE_OK != sqlite3_enable_load_extension(db, 1)){
	// 	printf("load error");
	// }
	
	// if(SQLITE_OK != sqlite3_load_extension(db, "./cksumvfs", 0, &errmsg)){
	// 	printf("error1 : %s\n", errmsg);
	//   	sqlite3_free(errmsg);
	// }

	sql = "PRAGMA integrity_check";
    if(SQLITE_OK != sqlite3_exec(db, sql, callback, 0, &errmsg)) {
        printf("error:%s\n",errmsg);
        sqlite3_free(errmsg);
    }
	

	int n = 8;
	int rc = sqlite3_file_control(db, 0, SQLITE_FCNTL_RESERVE_BYTES, &n);
	printf("rc: %d\n", rc);
	if(SQLITE_OK != sqlite3_exec(db, "VACUUM", 0, 0, &errmsg)){
		printf("error2 : %s\n", errmsg);
	  	sqlite3_free(errmsg);
	}
	
	sql = "PRAGMA checksum_verification=ON;";
    if(SQLITE_OK != sqlite3_exec(db, sql, callback, 0, &errmsg)) {
        printf("error:%s\n",errmsg);
        sqlite3_free(errmsg);
    }
	
	// char *sql;
	// sql = "SELECT count(*), verify_checksum(Lng) FROM gps GROUP BY 2;";
	// char **result;
	// int nrow, ncolumn;

	// if (sqlite3_get_table(db, sql, &result, &nrow, &ncolumn, &errmsg) != 0)
	// {
	//   	printf("error3 : %s\n", errmsg);
	//   	sqlite3_free(errmsg);
	// }
	
	// int index = ncolumn;

	// for(int i=0;i<nrow;i++){
	// 	for(int j=0;j<ncolumn;j++){
	// 		printf("%s : %s\n", result[j], result[index++]);
	// 	}
	// }
	// sqlite3_free_table(result);

	sqlite3_close(db);

	return 0;
}