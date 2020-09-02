#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h" // 包含系统参数定义,如 MAXARG

int getline(char *buf, int nbuf);  //函数声明

int main(int argc, char *argv[]) {
    // 检查参数个数是否正确,至少应该有 2 个参数(程序名和命令)
    if (argc < 2) {
        fprintf(2, "Usage: xargs command [args...]\n"); // 打印使用方法
        exit(1); // 退出程序
    }

    // 分配空间存储命令和参数
    char *cmd = argv[1]; // 命令
    char *args[MAXARG]; // 参数数组
    for (int i = 0; i < argc - 1; i++) {
        args[i] = argv[i + 1]; // 复制命令行参数到参数数组
    }
    args[argc - 1] = 0; // 最后一个参数置为 NULL,表示参数列表结束

    char buf[512]; // 缓冲区用于存储输入行
    int n, status;
    char *p;

    // 循环读取输入行并执行命令
    while ((n = getline(buf, sizeof(buf))) > 0) {
        buf[n - 1] = 0; // 替换换行符为空字符

        // 创建子进程
        int pid = fork();
        if (pid < 0) {
            fprintf(2, "xargs: fork failed\n"); // 打印错误信息
            exit(1); // 退出程序
        } else if (pid == 0) {
            // 子进程
            p = buf;
            args[argc - 1] = p; // 将输入行作为最后一个参数

            // 执行命令
            exec(cmd, args);
            fprintf(2, "xargs: exec failed\n"); // 如果执行失败,打印错误信息
            exit(1); // 退出子进程
        } else {
            // 父进程
            wait(&status); // 等待子进程执行完毕
        }
    }

    exit(0); // 正常退出程序
}

// 读取一行输入到缓冲区
int getline(char *buf, int nbuf) {
    int n = 0;
    char c;

    while (read(0, &c, 1) == 1) { // 从标准输入读取一个字符
        if (n >= nbuf - 1) {
            fprintf(2, "xargs: input line too long\n"); // 如果输入行过长,打印错误信息
            exit(1); // 退出程序
        }
        buf[n++] = c; // 将字符存储到缓冲区
        if (c == '\n') { // 如果遇到换行符,结束读取
            break;
        }
    }

    return n; // 返回实际读取的字符数
}