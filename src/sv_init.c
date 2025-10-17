/*
 * Copyright (C) 2025  Shu Jiarui
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
 
#include <stdlib.h>
#include <stdio.h>
#include <sqlite3.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>

#include "sv_init.h"
#include "symbol.h"

char err_buff[1024]={0};

int init_ok=0;

void* thread_exit_msg_success="thread exit with normal";
void* thread_exit_msg_failure="thread exit with error";
int log_login_fd=0;
int log_err_fd=0;

sqlite3* db_handle=NULL;

sv_socket_info_t sv_socket_info={0};

const char* get_datetime()   
{
    static char buffer[64];
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buffer, sizeof(buffer), "timestamp at %Y-%m-%d %H:%M:%S", tm_info);
    return buffer;
}

void __sv_write_log(const char* write_log,log_level level)
{
    if(0!=access(SERVER_PATH"/log",F_OK))
    {
        if(-1==mkdir(SERVER_PATH"/log",0755))
        {
            PRINT_ERROR(strerror(errno));
            printf("program exit...\n");
            return;
        }
    }

    debug_on();

    debug_print("access\n");
    
    const char *files[] = {
        SERVER_PATH"/log/server_error.log",
        SERVER_PATH"/log/access.log",
        SERVER_PATH"/log/start.log",
        SERVER_PATH"/log/sqlite.log"
    };


    int fd;
    debug_on();
    for(int i=0;i<4;i++)
    {
        if(0!=access(files[i],F_OK))
        {
            fd=open(files[i],O_CREAT|O_WRONLY|O_APPEND,0644);
            if(fd==-1)
            {
                PRINT_ERROR(strerror(errno));
                printf("program exit...\n");
                return;
            }
            debug_print("access\n");
            close(fd);
        }
    }
    

    if(level==LOG_SERVER_ERROR)
    {
        fd=open(files[0],O_WRONLY|O_APPEND);
    }
    else if(level==LOG_ACCESS)
    {
        fd=open(files[1],O_WRONLY|O_APPEND);
    }
    else if(level==LOG_START)
    {
        fd=open(files[2],O_WRONLY|O_APPEND);
    }
    else if(level==LOG_SQLITE)
    {
        fd=open(files[3],O_WRONLY|O_APPEND);
    }
    else
    {
        PRINT_ERROR("LOG LEVEL ERROR");
        printf("program exit...\n");
        return;
    }

    if(-1==fd)
    {
        PRINT_ERROR(strerror(errno));
        printf("program exit...\n");
        return;
    }

    size_t w_len=strlen(write_log);
    // if(write_log[w_len]!='\n')
    // {
    //     write_log[w_len]='\n';
    //     write_log[w_len+1]='0';
    // }
    if(w_len!=write(fd,write_log,w_len))
    {
        PRINT_ERROR(strerror(errno));
        printf("program exit...\n");
        return;
    }
    memset(err_buff,0,strlen(err_buff));
    return;
}


/*初始化rCloud工作目录初始化*/
void* __sv_dir_struct_init(void* __Nullable arg)
{
    char* dirs[5];
    dirs[0]=SERVER_PATH;
    dirs[1]=SERVER_PATH"/log";
    dirs[2]=SERVER_PATH"/data";
    dirs[3]=SERVER_PATH"/db";
    dirs[4]=SERVER_PATH"/swap";

    if(access(SERVER_PATH, F_OK)==0)
    {}
    else
    {
        if(-1==mkdir(SERVER_PATH,0777))
        {
            snprintf(err_buff,sizeof(err_buff),"mkdir: %s , %s \n",strerror(errno),get_datetime());
            __sv_write_log(err_buff,LOG_SERVER_ERROR);
            RETURN_FUNC(thread_exit_msg_failure);
        }
    }

    for(int i=1;i<5;++i)
    {
        if(access(dirs[i],F_OK)==0)
        {}
        else
        {
            if(-1==mkdir(dirs[i],0777))
            {
                snprintf(err_buff,sizeof(err_buff),"mkdir: %s , %s\n",strerror(errno),get_datetime());
                __sv_write_log(err_buff,LOG_SERVER_ERROR);
                RETURN_FUNC(thread_exit_msg_failure);
            }
        }
    }
    RETURN_FUNC(thread_exit_msg_success);
}



void* __sv_sqlite_init(void* __Nullable arg)
{
    int ret=sqlite3_open(SERVER_PATH"/db/rCloud.db", &db_handle);
    if(ret!=SQLITE_OK)
    {
        snprintf(err_buff,sizeof(err_buff),"%s,%s\n","sqlite3 open failure",get_datetime());
        __sv_write_log(err_buff,LOG_SQLITE);
        RETURN_FUNC(thread_exit_msg_failure);
    }

    char* sql_create_table="create table if not exists users(username varchar(64) primary key,password varchar(64));";
    char* errmsg=NULL;
    ret=sqlite3_exec(
        db_handle,
        sql_create_table,
        NULL,
        NULL,
        &errmsg
    );

    if(SQLITE_OK!=ret)
    {
        snprintf(err_buff,sizeof(err_buff),"%s,%s\n","sqlite3 create table failure",get_datetime());
        __sv_write_log(err_buff,LOG_SQLITE);
        RETURN_FUNC(thread_exit_msg_failure);
    }

    char* sql_cmd="select * from users;";
    char** rval=(void*)0;
    unsigned int rlen=0;
    unsigned int llen=0;

    ret=sqlite3_get_table(
        db_handle,
        sql_cmd,
        &rval,
        &rlen,
        &llen,
        &errmsg
    );

    for(int i=llen+1;i<=(rlen+1)*llen;++i)
    {
        if(strcmp("sys",rval[i-1])==0)
        {
            if(strcmp("!forgetpasswd",rval[i])==0)
            {
                sqlite3_free_table(rval);
                RETURN_FUNC(thread_exit_msg_success);
            }
        }            
    }
    ret=~SQLITE_OK;
    ret=sqlite3_exec(
        db_handle,
        "insert into users values('sys','!forgetpasswd')",
        NULL,
        NULL,
        &errmsg
    );
    if(SQLITE_OK!=ret)
    {
        snprintf(err_buff,sizeof(err_buff),"%s,%s\n",errmsg,get_datetime);
        __sv_write_log(err_buff,LOG_SQLITE);
        RETURN_FUNC(thread_exit_msg_failure);
    }
    sqlite3_free_table(rval);
    RETURN_FUNC(thread_exit_msg_success);
}

#if 1

extern void* __sv_socket_init(void* __Nullable arg)
{

    sv_socket_info.listen_fd=socket(AF_INET,SOCK_STREAM,0);
    if(sv_socket_info.listen_fd==-1)
    {
        snprintf(err_buff,sizeof(err_buff),"socket: %s , %s\n",strerror(errno),get_datetime());
        __sv_write_log(err_buff,LOG_SERVER_ERROR);
        RETURN_FUNC(thread_exit_msg_failure);
    }

    int opt_set_addr_reuse=1;
    if(-1==setsockopt(sv_socket_info.listen_fd,SOL_SOCKET,SO_REUSEADDR,&opt_set_addr_reuse,sizeof(opt_set_addr_reuse)))
    {
        snprintf(err_buff,sizeof(err_buff),"setsockopt: %s , %s\n",strerror(errno),get_datetime());
        __sv_write_log(err_buff,LOG_SERVER_ERROR);
        RETURN_FUNC(thread_exit_msg_failure);
    }

    sv_socket_info.listen_addr.sin_family=AF_INET;
    sv_socket_info.listen_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    sv_socket_info.listen_addr.sin_port=htons(LISTEN_PORT);
    sv_socket_info.max_listen_num=MAX_LISTEM_NUM;


    int ret=bind(sv_socket_info.listen_fd,(struct sockaddr*)&sv_socket_info.listen_addr,sizeof(sv_socket_info.listen_addr));
    if(-1==ret)
    {
        snprintf(err_buff,sizeof(err_buff),"bind: %s , %s\n",strerror(errno),get_datetime());
        __sv_write_log(err_buff,LOG_SERVER_ERROR);
        RETURN_FUNC(thread_exit_msg_failure);
    }

    ret=listen(sv_socket_info.listen_fd,sv_socket_info.max_listen_num);
    if(-1==ret)
    {
        snprintf(err_buff,sizeof(err_buff),"bind: %s , %s\n",strerror(errno),get_datetime());
        __sv_write_log(err_buff,LOG_SERVER_ERROR);
        RETURN_FUNC(thread_exit_msg_failure);
    }
    RETURN_FUNC(thread_exit_msg_success);
}



void __init()
{
    init_ok=0;
    pthread_t tid_dir;
    pthread_t tid_sqlite;
    pthread_t tid_sock;

    debug_on();

    if(0!=pthread_create(&tid_dir,NULL,__sv_dir_struct_init,NULL))
    {
        debug_print("dir_init_thread failure\n");
        exit(EXIT_FAILURE);
    }
    if(0!=pthread_create(&tid_sqlite,NULL,__sv_sqlite_init,NULL))
    {
        debug_print("sqlite_init_thread failure\n");
        exit(EXIT_FAILURE);
    }
    if(0!=pthread_create(&tid_sock,NULL,__sv_socket_init,NULL))
    {
        debug_print("sock_init_thread failure\n");
        exit(EXIT_FAILURE);
    }
    void *exit_st_dir;
    void *exit_st_sqlite;
    void *exit_st_sock;
    pthread_join(tid_dir,&exit_st_dir);
    pthread_join(tid_sqlite,&exit_st_sqlite);
    pthread_join(tid_sock,&exit_st_sock);

    debug_print("dir:%s\n",(char*)exit_st_dir);
    debug_print("sqlite:%s\n",(char*)exit_st_sqlite);
    debug_print("sock:%s\n",(char*)exit_st_sock);

    if(strcmp((char*)exit_st_dir,(char*)thread_exit_msg_success)==0)
    {
        init_ok=1;
    }
    else
    {
        snprintf(err_buff,sizeof(err_buff),"%s,%s\n","rCloud startup failure because of dir init failure",get_datetime());
        __sv_write_log(err_buff,LOG_START);
        exit(EXIT_FAILURE);
    }
    init_ok=0;
    if(strcmp((char*)exit_st_sqlite,(char*)thread_exit_msg_success)==0)
    {
        init_ok=1;
    }
    else
    {
        snprintf(err_buff,sizeof(err_buff),"%s,%s\n","rCloud startup failure because of sqlite init failure",get_datetime());
        __sv_write_log(err_buff,LOG_START);
        exit(EXIT_FAILURE);
    }
    init_ok=0;
    if(strcmp((char*)exit_st_sock,(char*)thread_exit_msg_success)==0)
    {
        init_ok=1;
    }
    else
    {
        snprintf(err_buff,sizeof(err_buff),"%s,%s\n","rCloud startup failure because of sock init failure",get_datetime());
        __sv_write_log(err_buff,LOG_START);
        exit(EXIT_FAILURE);
    }

    if(init_ok)
    {
        char startup[128]="rCloud server startup successfully ";
        strcat(startup,get_datetime());
        strcat(startup,"\n");
        __sv_write_log(startup,LOG_START);
    }
    debug_print("every init thread is ok\n");
}


#endif