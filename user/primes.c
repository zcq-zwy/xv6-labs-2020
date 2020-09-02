#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define PIPESIZE 2 // 定义管道大小为2,表示有读端和写端
#define READEND 0 // 定义读端的文件描述符为0
#define WRITEEND 1 // 定义写端的文件描述符为1

// prime函数用于创建每个筛选进程
void prime(int left_pipe[]) 
{
    int this_prime, right_pipe[PIPESIZE]; // 定义当前要筛选的素数和新管道
    // 从左侧管道读取当前要筛选的素数    
    close(left_pipe[WRITEEND]); // 关闭旧管道的写端
    pipe(right_pipe);  // 创建新管道
    if (read(left_pipe[READEND], &this_prime, 4) != 4) {
        exit(0); // 退出进程
    }
     
     
    printf("prime %d\n", this_prime); // 打印当前素数

    if (fork() == 0) { // 创建子进程
        // 子进程部分
        prime(right_pipe); // 递归调用prime函数,传入新管道
    } else {
        // 父进程部分
        close(right_pipe[READEND]); // 关闭新管道的读端
        int i, n; // 定义循环变量和读取数字的变量
        // 从旧管道读取剩余数字,如果不是当前素数的倍数就写入新管道
        for (i = 0; read(left_pipe[READEND], &n, 4) == 4; i++) {
            if (n % this_prime != 0) {
                write(right_pipe[WRITEEND], &n, 4);
            }
        }

        close(left_pipe[READEND]); // 关闭旧管道的读端
        close(right_pipe[WRITEEND]); // 关闭新管道的写端
        wait(0); // 等待子进程退出
    }
}

int main(void)
{
    int pipe0[PIPESIZE]; // 定义初始管道
    if (pipe(pipe0) < 0) { // 创建初始管道
        fprintf(2, "pipe failed\n"); // 如果创建失败,打印错误信息
        exit(1); // 退出进程
    }
    if (fork() == 0) { // 创建子进程
        // 子进程部分
        prime(pipe0); // 调用prime函数,传入初始管道
    } else {
        // 父进程部分
        close(pipe0[READEND]); // 关闭初始管道的读端
        // 将数字2到35写入初始管道
        for (int i = 2; i <= 35; i++) {
            write(pipe0[WRITEEND], &i, 4);
        }
        close(pipe0[WRITEEND]); // 关闭初始管道的写端
        wait(0); // 等待子进程退出
        
    }

    exit(0); // 退出主进程
}