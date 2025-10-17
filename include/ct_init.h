
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

extern sem_t init_semaphore;

extern char* error_type[];

extern char* thread_exit_msg_success;
extern char* thread_exit_msg_failure;

extern int g_confd;
__Heapable extern void* g_conf_path;
__Heapable extern ct_conf_info_t* g_conf_info;

#ifdef LOGIN_NAME_MAX
#define CT_USERRC_MAX (LOGIN_NAME_MAX+16)
#else
#define CT_USERRC_MAX (256+16)
#endif
#define CT_USERNAME_MAX (64)
#define CT_USERPASSWD_MAX (64)


#define PRINT_ERROR(STR) fprintf(stderr,"%s->%s-%s-%d",STR,__FILE__,__FUNCTION__,__LINE__)

//获取配置文件所在的路径
__Heapable extern int __userrc_get();
//检测是否有配置文件?
extern void __is_confd(char* _conf_path);
//初始化配置文件
extern int __init_conf(char* _conf_path);
//conf文件解析cJSON
extern int __parse_conf(char* _conf_path);
//初始化
extern void* __init(void* arg);

void* ct_init(void*);



#endif