#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    int parent_to_child[2]; // 父进程向子进程发送数据的管道
    int child_to_parent[2]; // 子进程向父进程发送数据的管道
    char buffer[1]; // 用于存储通信字节
    int pid;

    // 创建两个管道
    if (pipe(parent_to_child) < 0 || pipe(child_to_parent) < 0) {
        fprintf(2, "pipe failed\n");
        exit(1);
    }

    // 创建子进程
    pid = fork();
    if (pid < 0) {
        fprintf(2, "fork failed\n");
        exit(1);
    } else if (pid == 0) { // 子进程
        // 关闭不需要的管道端
        close(parent_to_child[1]);
        close(child_to_parent[0]);

        // 从父进程读取数据
        if (read(parent_to_child[0], buffer, 1) != 1) {
            fprintf(2, "read from parent failed\n");
            exit(1);
        }
        printf("%d: received ping\n", getpid());

        // 向父进程写入数据
        if (write(child_to_parent[1], buffer, 1) != 1) {
            fprintf(2, "write to parent failed\n");
            exit(1);
        }

        // 关闭管道端并退出子进程
        close(parent_to_child[0]);
        close(child_to_parent[1]);
        exit(0);
    } else { // 父进程
        // 关闭不需要的管道端
        close(parent_to_child[0]);
        close(child_to_parent[1]);

        // 向子进程写入数据
        buffer[0] = 'a';
        if (write(parent_to_child[1], buffer, 1) != 1) {
            fprintf(2, "write to child failed\n");
            exit(1);
        }

        // 从子进程读取数据
        if (read(child_to_parent[0], buffer, 1) != 1) {
            fprintf(2, "read from child failed\n");
            exit(1);
        }
        printf("%d: received pong\n", getpid());

        // 关闭管道端并等待子进程退出
        close(parent_to_child[1]);
        close(child_to_parent[0]);
        wait(0);
    }
    

    exit(0);
}