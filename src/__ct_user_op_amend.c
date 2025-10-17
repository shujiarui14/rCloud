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


#include "ct_user_op.h"
#include "ct_init.h"
#include <unistd.h>
#include <fcntl.h>

/*
@brief 修改配置文件，包含服务器的IP、端口，用户名、用户密码

@param __connfd,任意数，函数内内不会使用，只是为了匹配函数签名

@param __filepath,配置文件的路径，ct_init.h 有定义，可以不填

@return EXIT_FAILURE修改失败，EXIT_SUCCESS修改成功
*/
int __ct_user_op_amend(const int __connfd,const char* __Nullable __filepath)
{
    char server_ip[16];
    unsigned short server_port;
    char username[CT_USERNAME_MAX];
    char userpasswd[CT_USERPASSWD_MAX];
    do
    {
        printf("Please input server ip:");
        fgets(server_ip,sizeof(server_ip),stdin);
        if(strlen(server_ip)>16||strlen(server_ip)<1)
        {
            printf("IP format is wrong");
            break;
        }
        else
        {
            /*把\n替换成\0*/
            server_ip[strcspn(server_ip,"\n")]='\0';
        }

        printf("Please input server port:");
        scanf("%hu",&server_port);
        if(server_port<1025||server_port>65535)
        {
            printf("Server port must be in the range 1024–65535!");
            break;
        }
        else
        {
            /*清除缓冲区的回车*/
            getchar();
        }

        printf("Please input username:");
        fgets(username,sizeof(username),stdin);
        if(strlen(username)>CT_USERNAME_MAX||strlen(username)<1)
        {
            printf("Username must be in the range %d to %d",0,CT_USERNAME_MAX);
            break;
        }
        else
        {
            username[strcspn(username,"\n")]='\0';
        }
       
        printf("Please input userpasswd:");
        fgets(userpasswd,sizeof(userpasswd),stdin);
        if(strlen(userpasswd)>CT_USERPASSWD_MAX||strlen(userpasswd)<1)
        {
            printf("Usernpasswd must be in the range %d to %d",0,CT_USERPASSWD_MAX);
            break;
        }
        else
        {
            userpasswd[strcspn(userpasswd,"\n")]='\0';
        }

        /* 以cJSON格式存储*/
        cJSON* userrc=cJSON_CreateObject();
        cJSON_AddStringToObject(userrc,"server_ip",server_ip);
        cJSON_AddNumberToObject(userrc,"server_port",server_port);
        cJSON_AddStringToObject(userrc,"username",username);
        cJSON_AddStringToObject(userrc,"userpasswd",userpasswd);

        char *json_str = cJSON_Print(userrc);
        size_t userrc_len=strlen(json_str);

        int conf_fd=-1;
        if(__filepath==NULL)
        {
            conf_fd=open(g_conf_path,O_WRONLY|O_CREAT|O_TRUNC,0644);
        }
        else
        {
            conf_fd=open(__filepath,O_WRONLY|O_CREAT|O_TRUNC,0644);
        }

        
        if(-1==conf_fd)
        {
            PRINT_ERROR(error_type[3]);
            return EXIT_FAILURE;
        }

        if(userrc_len!=write(conf_fd,json_str,userrc_len))
        {
            PRINT_ERROR(error_type[1]);
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }while(0);
    printf("rCloud will exit now...\n");
    return EXIT_FAILURE;
}