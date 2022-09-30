#include "sqlite3.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    int i;
    for (int i=0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");  
    }
    printf("\n");
    return 0;
}
struct callbackres {
	char res[1024*1024*2];
	int res_size;
};

static int callback2(void *cres, int argc, char **argv, char **azColName)
{
	int n = ((struct callbackres *)cres)->res_size;
	char *res = ((struct callbackres *)cres)->res;
	for (int i=0; i<argc; i++) {
		n += sprintf(res+n, "%s : %s\n", azColName[i], argv[i]?argv[i]:"NULL");
	}
    ((struct callbackres *)cres)->res_size = n;
	return 0;
}

void main(){
    
    sqlite3 * db=NULL;
    int rc =0;
    char *zErrMsg = NULL;
    sqlite3 *db2;
    char *sql=NULL;
    clock_t start, finish;
    struct timeval start2,end2;
    double Total_time,persql_time;
    int count_N=0;
    struct callbackres cres;
    char *dbname="./aes_cbc_128.db";
    // char*dbname="/data/aes_cbc_128.db";
    // char*dbname="/data/secret.db";
    long timeuse=0;
    memset(&cres, 0, sizeof(cres));
    sqlite3_open(":memory:", &db2);
    rc=sqlite3_enable_load_extension(db2,1);
     if (rc != 0) {
        printf("SQL error:%s\n",zErrMsg);
        sqlite3_free(zErrMsg);
    }
    rc = sqlite3_load_extension(db2, "./cksumvfs",0,&zErrMsg);//本地在makefile已经制定了链接位置
    // rc = sqlite3_load_extension(db2, "lib/cksumvfs",0,&zErrMsg);
    if (rc != 0) {
        printf("SQL error:%s\n",zErrMsg);
        sqlite3_free(zErrMsg);
    }
    printf("sqlite3_load_extension rc=%d\n",rc);
    sqlite3_close(db2);
    
    rc = sqlite3_open(dbname, &db);
    if (rc) {
        perror("open sqlite_open ");
        sqlite3_close(db);
    }
    printf("PRAGMA checksum_verification:");
    sql = "PRAGMA checksum_verification=ON;";
    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if (rc != 0) {
        printf("SQL error:%s\n",zErrMsg);
        sqlite3_free(zErrMsg);
    }
    sql = "PRAGMA journal_mode  ;";
      rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if (rc != 0) {
        printf("SQL error:%s\n",zErrMsg);
        sqlite3_free(zErrMsg);
    }

    int n = 8;
    rc=sqlite3_file_control(db, NULL, SQLITE_FCNTL_RESERVE_BYTES, &n);
    printf("sqlite3_file_control rc=%d\n",rc);
    // sql="PRAGMA page_size=4096;";
    // rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    // if (rc != 0) {
    //     printf("PRAGMA  page_size SQL error:%s\n",zErrMsg);
    //     // sqlite3_free(zErrMsg);
    // }
    // printf("PRAGMA  page_size rc=%d\n",rc);
    // printf("exec  sql  VACUUM\n");
    //  rc=sqlite3_exec(db, "VACUUM", 0, 0, 0);
    // if (rc != 0) {
    //     printf("VACUUM SQL error:%s\n",zErrMsg);
    //     sqlite3_free(zErrMsg);
    // }
    
    //验证checksum_verification已经开启
    sql="PRAGMA integrity_check";
   //sql = "SELECT  verify_checksum(id=0) FROM gps;";
    //sql = "PRAGMA checksum_verification;";
    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if (rc != 0) {
        printf("SQL error:%s\n",zErrMsg);
        sqlite3_free(zErrMsg);
    }
     printf("PRAGMA checksum_verification:");
    sql = "PRAGMA checksum_verification=ON;";
    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if (rc != 0) {
        printf("SQL error:%s\n",zErrMsg);
        sqlite3_free(zErrMsg);
    }
    //测试查询语句开始
   gettimeofday(&start2, NULL);
     //start = clock();
    sql = "select * from gps limit 100";
    rc = sqlite3_exec(db, sql, callback2, (void *)&cres, &zErrMsg);
    if (rc != 0) {
        printf("SQL error:%s\n",zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else{
        char databuff[1];
        // getpageData(db, dbname,2,databuff);
        //测试结束
        sqlite3_close(db);
        gettimeofday(&end2, NULL);
        timeuse =1000000 * ( end2.tv_sec - start2.tv_sec ) + end2.tv_usec - start2.tv_usec;
        printf("SQL更新语句总共所用时间time=%lds\n", end2.tv_sec - start2.tv_sec);
        printf("进行SQL语句:%s\n所用时间为%f ms\n",sql,timeuse /1000.0);
        persql_time=timeuse/100;
        printf("每条SQL查询语句所用时间：%fms\n",persql_time/1000);
    }
    //测试结束



    //测试update语句开始
    char sql2[50];
    rc = sqlite3_open(dbname, &db);
        if (rc) {
            perror("open sqlite_open ");
            sqlite3_close(db);
    }
    for(int i=1;i<6;i++){
        gettimeofday(&start2, NULL);
        sprintf(sql2,"update gps set  TimeUS=411364023 where Id=%d",i);
        rc = sqlite3_exec(db, sql2, callback2, 0, &zErrMsg);
        if (rc != 0) {
            printf("SQL error:%s\n",zErrMsg);
            sqlite3_free(zErrMsg);
        }
            printf("sql:%s\n",sql2);
    }
    sqlite3_close(db);
    gettimeofday(&end2, NULL);
    
    timeuse =1000000 * ( end2.tv_sec - start2.tv_sec ) + end2.tv_usec - start2.tv_usec;
    printf("SQL更新语句总共所用时间time=%fms\n",timeuse /1000.0);        
    printf("每条SQL更新语句所用时间time=%fms\n",timeuse /1000.0/5);
}
