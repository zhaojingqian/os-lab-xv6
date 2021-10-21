#include "kernel/types.h"
#include "user/user.h"

int main(int argc,char* argv[]) {
    int fds1[2];
    int fds2[2];
    int pid;

    //创建管道
    if(pipe(fds1) < 0) {
        printf("pipe() failed\n");
        exit(1);
    }
    if(pipe(fds2) < 0) {
        printf("pipe() failed\n");
        exit(1);
    }
    //创建子进程
    if((pid = fork()) < 0) {
        printf("fork() failed\n");
        exit(1);
    } else if(pid == 0) {
        //子进程
        //从管道1读出
        char str1[50];
        close(fds1[1]); //关闭管道1写端
        read(fds1[0], str1, 50); //读取管道至str1
        close(fds1[0]); //关闭读端
        printf("%d: received %s\n", getpid(), str1);

        //将“pong”写入管道2
        char string1[50] = "pong";
        close(fds2[0]); //关闭读端
        write(fds2[1], string1, 50);
        close(fds2[1]);

        exit(0);
    } else {        
        //父进程
        //将“ping”写入管道1
        char str2[50] = "ping";
        close(fds1[0]); //关闭读端
        write(fds1[1], str2, 50);
        close(fds1[1]);

        wait(0);

        close(fds2[1]); //关闭管道2写端
        read(fds2[0], str2, 50); //读取管道至str2
        close(fds2[0]); //关闭读端
        printf("%d: received %s\n",getpid(), str2);

        exit(0);
    }
}
