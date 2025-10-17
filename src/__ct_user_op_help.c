#include "ct_user_op.h"

int __ct_user_op_help(const int __connfd, const char* __Nullable __filepath)
{
    // ANSI颜色
    const char* RED    = "\033[31m";
    const char* GREEN  = "\033[32m";
    const char* YELLOW = "\033[33m";
    const char* CYAN   = "\033[36m";
    const char* RESET  = "\033[0m";

    printf("%sNAME\n\t%s rCloud\n", 
           GREEN, RED);

    printf("%sSYNOPSIS%s\n\t%s rCloud [OPTIONS] [FILEPATH]%s\n\n", 
           GREEN, RESET, CYAN, RESET);

    printf("%sDESCRIPTION%s\n\t%s rCloud is a remote cloud storage system,which can store and manage your files and directories on your server.%s\n\n",
           GREEN, RESET, CYAN, RESET);

    printf("%sOPTIONS%s\n", GREEN, RESET);

    printf("\t%s-a%s : amend\n\t\tModify the configuration file (~/.ct_user.rc)\n\n",
           YELLOW, RESET);

    printf("\t%s-u%s : upload [UPFILENAMES REQUIRED] [SAVEPATH REQUIRED]\n\t\tUpload a file or directory to the cloud server\n\n",
           YELLOW, RESET);

    printf("\t%s-d%s : download [DOWNFILENAMES REQUIRED] [SAVEPATH REQUIRED]\n\t\tDownload a file or directory from the cloud server\n\n",
           YELLOW, RESET);

    printf("\t%s-l%s : list [DIR PATH REQUIRED]\n\t\tList files and directories on the cloud server\n\n",
           YELLOW, RESET);

    printf("\t%s-r%s : rename [PATH REQUIRED] [NEW NAME REQUIRED]\n\t\tRename a file or directory on the cloud server\n\n",
           YELLOW, RESET);

    printf("\t%s-o%s : online\n\t\tTest the connection to cloud server\n\n",
           YELLOW, RESET);

    printf("\t%s-e%s : erase [PATH REQUIRED]\n\t\tErase a file or directory on the cloud server\n\n",
           YELLOW, RESET);

    printf("\t%s-h%s : help\n\t\tShow this help list\n\n",
           YELLOW, RESET);

    printf("\t%s-s%s : sign up\n\t\tSign up a new account on the cloud server\n\n",
           YELLOW, RESET);

    printf("\t%s-c%s : cancel\n\t\tCancel the account on the cloud server\n\n",
           YELLOW, RESET);

    return EXIT_SUCCESS;
}
