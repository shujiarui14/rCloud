#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "ct_init.h"
#include "symbol.h"
#include "cJSON.h"

int g_confd=0;
__Heapable void* g_conf_path=NULL;
__Heapable ct_conf_info_t* g_conf_info=NULL;


char* thread_exit_msg_success="thread exit with normal";
char* thread_exit_msg_failure="thread exit with error";

__Heapable int __userrc_get()
{
    __Heapable char* userrc=(char*)malloc(CT_USERRC_MAX);
    if(NULL==userrc)
    {
        PRINT_ERROR("malloc");
        return -1;
    }
    snprintf(userrc,CT_USERRC_MAX,"%s/.ct_user.rc",getenv("HOME"));
    g_conf_path=userrc;
    return 0;
}


void __is_confd(char* _conf_path)
{
    if(NULL==_conf_path)
        raise(SIGSEGV);
    if(0==access(_conf_path,F_OK))
    {
        g_confd=1;
    }
}

int __init_conf(char* _conf_path)
{
    char server_ip[16];
    unsigned short server_port;
    char username[CT_USERNAME_MAX];
    char userpasswd[CT_USERPASSWD_MAX];

    int conf_fd=open(_conf_path,O_WRONLY|O_CREAT|O_TRUNC,0644);
    if(-1==conf_fd)
    {
        PRINT_ERROR("open");
        return EXIT_FAILURE;
    }

    //这最好做个判断，数据是否合法
    printf("Please input server ip:");
    fgets(server_ip,sizeof(server_ip),stdin);
    server_ip[strcspn(server_ip,"\n")]='\0';
    printf("Please input server port:");
    scanf("%hu",&server_port);
    getchar();
    printf("Please input username:");
    fgets(username,sizeof(username),stdin);
    username[strcspn(username,"\n")]='\0';
    printf("Please input userpasswd:");
    fgets(userpasswd,sizeof(userpasswd),stdin);
    userpasswd[strcspn(userpasswd,"\n")]='\0';

    cJSON* userrc=cJSON_CreateObject();
    cJSON_AddStringToObject(userrc,"server_ip",server_ip);
    cJSON_AddNumberToObject(userrc,"server_port",server_port);
    cJSON_AddStringToObject(userrc,"username",username);
    cJSON_AddStringToObject(userrc,"userpasswd",userpasswd);

    char *json_str = cJSON_Print(userrc);
    size_t userrc_len=strlen(json_str);

    if(userrc_len!=write(conf_fd,json_str,userrc_len))
    {
        PRINT_ERROR("write");
        return EXIT_FAILURE;
    }
    //printf("%s\n",json_str);
#if 0
#endif
    return EXIT_SUCCESS;
}


//获取配置文件信息
__Heapable int __parse_conf(char* _conf_path)
{
    if(NULL==_conf_path)
    {
        raise(SIGSEGV);
    }

    int _confd=open((char*)_conf_path,O_RDONLY);
    if(-1==_confd)
    {
        PRINT_ERROR("open");
        return EXIT_FAILURE;
    }

    g_conf_info=(ct_conf_info_t*)malloc(sizeof(ct_conf_info_t));
    if(NULL==g_conf_info)
    {
        raise(SIGSEGV);
    }

    char _conf_str[512]={0};
    int read_size=0;
    if(-1==(read_size=read(_confd,_conf_str,sizeof(_conf_str)-1)))
    {
        PRINT_ERROR("read");
        return EXIT_FAILURE;
    }
    _conf_str[read_size]='\0';
    // 通过 cJSON_Parse() 函数将 JSON 字符串转换为 cJSON 对象
    cJSON* userrc = cJSON_Parse(_conf_str);
    // 使用 cJSON_GetObjectItem() 获取具体字段
    cJSON* server_ip = cJSON_GetObjectItem(userrc, "server_ip");
    if (server_ip && cJSON_IsString(server_ip) && server_ip->valuestring != NULL) 
    {
        g_conf_info->server_ip=server_ip->valuestring;
    }

    cJSON* server_port = cJSON_GetObjectItem(userrc, "server_port");
    if(server_port && cJSON_IsNumber(server_port))
    {
        g_conf_info->server_port=server_port->valueint;
    }

    cJSON* username = cJSON_GetObjectItem(userrc, "username");
    if(username && cJSON_IsString(username) && username->valuestring != NULL)
    {
        g_conf_info->username=username->valuestring;
    }

    cJSON* userpasswd = cJSON_GetObjectItem(userrc, "userpasswd");
    if(userpasswd && cJSON_IsString(userpasswd) && userpasswd->valuestring != NULL)
    {
        g_conf_info->userpasswd=userpasswd->valuestring;
    }
    g_conf_info->userrc=userrc;
    close(_confd);
}


void* __init(void* arg)
{
    __userrc_get();
    __is_confd((char*)g_conf_path);
    int ret=-1;
    if(!g_confd)
    {
        ret=__init_conf((char*)g_conf_path);
        if(EXIT_SUCCESS!=ret)
        {
            PRINT_ERROR("conf init failure");
            pthread_exit((void*)thread_exit_msg_failure);
        }
    }
    __parse_conf((char*)g_conf_path);
    //return (void*)thread_exit_msg_success;
    pthread_exit((void*)thread_exit_msg_success);
}







