#ifndef __SYMBOL_H__
#define __SYMBOL_H__

#if defined(__has_include)
    #if __has_include(<shujiarui/debug.h>)
        #include <shujiarui/debug.h>
        #define HAS_SHUJIARUI 1
    #else
        #define HAS_SHUJIARUI 0
    #endif
#else
    #define HAS_SHUJIARUI 0
#endif

#if HAS_SHUJIARUI
#else
#define debug_on() 
#define debug_off() 
#define debug_print(fmt,...) 
#define debug_print_with_log(fmt,...) 
#define debug_execute(...) 
#define debug_assert(expr) 
#endif

#define PRINT_ERROR(STR) fprintf(stderr,"%s->%s-%s-%d",STR,__FILE__,__FUNCTION__,__LINE__)

#ifndef __Nullable
#define __Nullable
#endif

#ifndef __Heapable
#define __Heapable
#endif

#define strong_alias(name,aliasname) \
    extern typeof(name) aliasname __attribute__((alias(#name)))

typedef enum _sc_request
{
    LOGIN=0x00,
    UPLOAD=0x01,
    DOWNLOAD=0x02,
    LIST=0x03,
    MOVE=0x04,
    ONLINE=0x05,
    ERASE=0x06,
    SING_UP=0x07,
    CANCEL=0x08
}sv_request_t;

typedef enum __log_level
{
    LOG_SERVER_ERROR=0x00,
    LOG_ACCESS=0x01,
    LOG_START=0x02,
    LOG_SQLITE=0x03
}log_level;


#endif
