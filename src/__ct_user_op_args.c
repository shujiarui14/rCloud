
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "ct_user_op.h"
#include "symbol.h"
#include "ct_init.h"

__Heapable user_op_args_t* __ct_user_op_args=NULL;

__Heapable void* __resolve_symbol(int argc,char** argv)
{
    
    int opt;
    __ct_user_op_args=(user_op_args_t*)malloc(sizeof(user_op_args_t));
    memset(__ct_user_op_args, 0, sizeof(user_op_args_t));
    opt=getopt(argc,argv,"au:d:l:m:oe:hsc");
    switch(opt)
    {
        case 'a':
            __ct_user_op_args->_ct_op='a';
            __ct_user_op_args->_ct_path2=NULL;
            __ct_user_op_args->_ct_path2=NULL;        
            return thread_exit_msg_success;
        case 'u':
            __ct_user_op_args->_ct_op='u';
            __ct_user_op_args->_ct_path1=optarg;
            __ct_user_op_args->_ct_path2=argv[optind];
            return thread_exit_msg_success;
        case 'd':
            __ct_user_op_args->_ct_op='d';
            __ct_user_op_args->_ct_path1=optarg;
            __ct_user_op_args->_ct_path2=argv[optind];
            return thread_exit_msg_success;
        case 'l':
            __ct_user_op_args->_ct_op='l';
            __ct_user_op_args->_ct_path1=optarg;
            __ct_user_op_args->_ct_path2=NULL;
           return thread_exit_msg_success;
        case 'm':
            __ct_user_op_args->_ct_op='m';
            __ct_user_op_args->_ct_path1=optarg;
            __ct_user_op_args->_ct_path2=argv[optind];
            return thread_exit_msg_success;
        case 'o':
            __ct_user_op_args->_ct_op='o';
            __ct_user_op_args->_ct_path2=NULL;
            __ct_user_op_args->_ct_path2=NULL;
            return thread_exit_msg_success;
        case 'e':
            __ct_user_op_args->_ct_op='e';
            __ct_user_op_args->_ct_path1=optarg;
            __ct_user_op_args->_ct_path2=NULL;
            return thread_exit_msg_success;
        case 'h':
            __ct_user_op_args->_ct_op='h';
            __ct_user_op_args->_ct_path2=NULL;
            __ct_user_op_args->_ct_path2=NULL;
            return thread_exit_msg_success;
        case's':
            __ct_user_op_args->_ct_op='s';
            __ct_user_op_args->_ct_path2=NULL;
            __ct_user_op_args->_ct_path2=NULL;
            return thread_exit_msg_success;
        case 'c':
             __ct_user_op_args->_ct_op='c';
            __ct_user_op_args->_ct_path2=NULL;
            __ct_user_op_args->_ct_path2=NULL;
            return thread_exit_msg_success;
        default:
            return thread_exit_msg_failure;
    }
}

