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
 
#ifndef __SV_INIT_H__
#define __SV_INIT_H__

#include <netinet/in.h>
#include <sqlite3.h>
#include "symbol.h"

#define __THREAD_ON
#ifdef __THREAD_ON
#define RETURN_FUNC(PARAMETER) pthread_exit(PARAMETER)
#else
#define RETURN_FUNC(PARAMETER) return PARAMETER
#endif


#define MAX_LISTEN_NUM (32)
#define LISTEN_PORT (8192)

extern int init_ok;

#define SERVER_PATH "/rCloud"

extern char err_buff[1024];

extern void* thread_exit_msg_success;
extern void* thread_exit_msg_failure;

__Heapable extern sqlite3* db_handle;

#define MAX_LISTEM_NUM (16)

// 监听文件描述符
typedef struct _sv_socket_info
{
    int listen_fd;
    size_t max_listen_num;
    struct sockaddr_in listen_addr;
}sv_socket_info_t;

extern sv_socket_info_t sv_socket_info;

//文件夹结构目录初始化
extern void* __sv_dir_struct_init(void* __Nullable arg);
//数据库初始化
extern void* __sv_sqlite_init(void* __Nullable arg);
//套接字监听初始化
extern void* __sv_socket_init(void* __Nullable arg);
//初始化操作
extern void __init() __attribute((weak));
//记录日志
extern void __sv_write_log(const char* write_log,log_level level);
//获取时间戳的函数
extern const char* get_datetime(); 


#endif
 
