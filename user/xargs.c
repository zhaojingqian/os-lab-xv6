#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

// !xarg功能
// !执行xarg后面的指令


void main(int argc, char *argv[]) {
    //xargs echo 1 2
    //echo 3 4 |xargs echo 1 2
    //echo hello | xargs echo 1 2
    //echo hello world | xargs echo 1 2
    //echo 3 4 hello world | xargs echo 1 2
    
    char *pargv[MAXARG];
    int init_argc = --argc;
    for(int i=0; i<argc; i++) {
        pargv[i] = argv[i+1];
    }
    // char **pargv = argv + 1;
    // --argc;

    // * argv[1] 是要执行的指令名称
    // * 后面是参数

    // !对管道读取字符的处理

                // printf("before:argc=%d\n", argc);
                // for(int i=0; i<argc; i++) {
                //     printf("%s\n", pargv[i]);
                // }
                // printf("\n");

    char *inst = argv[1];  
    // *应从标准输入中读取管道，大小未知
    // int i = argc;
    char ch;
    char buf[512];
    char *pbuf = buf;
    int count = -1;
    int pid;
    while(read(0, &ch, 1) > 0) {
                // printf("ch=%c\n", ch);
        // !判断ch
        if(ch == ' ') { // !空格表示前面已经形成字符串，赋给pargv
            buf[++count] = 0;
                // printf("-buf=%s\n", pbuf);
            pargv[argc++] = pbuf;
            pbuf = buf+count+1;         
                // printf("argc=%d\n", argc);

                // for(int i=0; i<argc; i++) {
                //     printf("%s \n", pargv[i]);
                // }
                // printf("\n");
        } else if(ch == '\n') { // !传递结束，可以执行
            buf[++count] = 0;
                // printf("--buf=%s\n", pbuf);
            pargv[argc++] = pbuf;
            pbuf = buf+count+1;
            pargv[argc] = 0;
                // printf("argc=%d\n", argc);

                // for(int i=0; i<argc; i++) {
                //     printf("%s\n", pargv[i]);
                // }
                // printf("\n");   
                // break;      
            if((pid = fork()) < 0) {
                    printf("fork() fail\n");
                    exit(1);

                } else if(pid == 0) {

                            // printf("inst=%s \n", inst);
                            // for(int i=0; i<argc; i++) {
                            //     printf("%s\n", pargv[i]);
                            // }
                            // printf("\n"); 

                    exec(inst, pargv);

                    exit(0);
                } else {
                    wait(0);
                    // !继续初始化
                    argc = init_argc;
                    count = -1;
                    pbuf = buf;
                }
        } else {
            buf[++count] = ch;
                // printf("-ch=%c\n", ch);
        }
    }
                    // printf("after:argc=%d\n", argc);
                    // for(int i=0; i<argc; i++) {
                    //     printf("%s\n", pargv[i]);
                    // }
                    // printf("\n");
    exit(0);
}