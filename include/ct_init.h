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



#ifndef __CT_INIT_H__
#define __CT_INIT_H__

#include <limits.h>
#include <semaphore.h>
#include "cJSON.h"
#include "symbol.h"



typedef struct _ct_conf_info
{
    cJSON* userrc;//org info to server auth
    char* server_ip;
    unsigned short server_port;
    char* username;
    char* userpasswd;
} ct_conf_info_t;

//信号量，当初始化操作完成后会post，有些操作可能必须发生在初始化操作之后
extern sem_t init_semaphore;

//错误类型
extern char* error_type[];

//线程退出信息，必要时会查看线程退出状态
extern char* thread_exit_msg_success;
extern char* thread_exit_msg_failure;

//判断是否第一次在这台机器上运行
extern int g_confd;

//配置文件所在的路径
__Heapable extern void* g_conf_path;

/*
* 配置文件信息，每次会在init获取
* 在连接服务的时候需要用这些信息做验证
*/
__Heapable extern ct_conf_info_t* g_conf_info;

/*
* 配置文件的最大查找路径，根据系统登陆的用户名的最大值
*/
#ifdef LOGIN_NAME_MAX
#define CT_USERRC_MAX (LOGIN_NAME_MAX+16)
#else
#define CT_USERRC_MAX (256+16)
#endif

/*用户名的最大长度*/
#define CT_USERNAME_MAX (64)
/*用户密码的最大长度*/
#define CT_USERPASSWD_MAX (64)


#define PRINT_ERROR(STR) fprintf(stderr,"%s->%s-%s-%d",STR,__FILE__,__FUNCTION__,__LINE__)

//获取配置文件所在的路径
__Heapable int __userrc_get() __attribute__((visibility("hidden")));
//检测是否有配置文件?
 void __is_confd(char* _conf_path) __attribute__((visibility("hidden")));
//初始化配置文件
 int __init_conf(char* _conf_path) __attribute__((visibility("hidden")));
//conf文件解析cJSON
 int __parse_conf(char* _conf_path) __attribute__((visibility("hidden")));
//初始化
 void* __init(void* arg) __attribute__((visibility("hidden")));

//留给mian的初始化，是有__init取得别名，所有__开头的函数不能被其他文件所调用
void* ct_init(void*);



#endif