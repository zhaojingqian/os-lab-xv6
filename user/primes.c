#include "kernel/types.h"
#include "user/user.h"

void pprime(int fd[2]) {
    int pid;
    int prime;
    int num;
    int fds[2];

    //关闭写端
    close(fd[1]);

    if(!read(fd[0], &prime, 4)) {
        close(fd[0]);
        exit(0);
    } else {
        printf("prime %d\n", prime);
        pipe(fds);
        // printf("fd[0]=%d, fd[1]=%d\tfds[0]=%d, fds[1]=%d\n",fd[0], fd[1], fds[0], fds[1]);
        if((pid = fork()) < 0) {
            printf("fork() failed\n");
            exit(1);
        } else if(pid == 0) {
            close(fd[0]);
            pprime(fds);
        } else {
            close(fds[0]);
            while(read(fd[0], &num, 4)) {
                if(num % prime)
                    write(fds[1], &num, 4);
            }
            //传输完毕
            close(fd[0]);
            close(fds[1]);
            wait(0);
            exit(0);
        }
    }
}

void main(int argc,char* argv[]) {
    //每个进程接受数据后选择第一个数据输出，
    //筛掉该数据的倍数
    //将剩下的数据传给下一个进程

    // printf("size of int is %d\n", sizeof(int));

    int pid;
    int fd[2];
    pipe(fd);

    if((pid = fork()) < 0) {
        printf("fork() failed\n");
        exit(1);
    } else if(pid == 0) {    
        pprime(fd);
    } else {
        close(fd[0]);
        int prime = 2;
        printf("prime %d\n", prime);
        int num = prime + 1;
        while(num != 36) {
            if(num % prime) {
                write(fd[1], &num, 4);
            }
            ++num;
        }
        close(fd[1]); //传输结束
        wait(0); //等待子进程结束
        exit(0);
    }
}