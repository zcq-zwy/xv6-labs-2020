#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h" // 包含文件系统相关的数据结构和常量定义

// 函数声明
void find(char *path, char *filename);

int main(int argc, char *argv[]) {
    // 检查参数个数是否正确,应该有 3 个参数(程序名、目录路径、文件名)
    if (argc != 3) {
        fprintf(2, "Usage: find directory filename\n"); // 打印使用方法
        exit(1); // 退出程序
    }

    // 获取目录和文件名参数
    char *directory = argv[1];
    char *filename = argv[2];

    // 调用 find 函数
    find(directory, filename);
    exit(0); // 正常退出程序
}

void find(char *path, char *filename) {
    char buf[512], *p; // buf 用于存储当前文件/目录的完整路径
    int fd; // 目录文件描述符
    struct dirent de; // 目录项结构体
    struct stat st; // 文件状态结构体

    // 打开目录
    if ((fd = open(path, 0)) < 0) { // 以只读方式打开目录
        fprintf(2, "find: cannot open %s\n", path); // 打印错误信息
        return; // 退出函数
    }

    // 获取目录的状态信息
    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path); // 打印错误信息
        close(fd); // 关闭目录文件描述符
        return; // 退出函数
    }

    // 将路径拼接到 buf 中
    strcpy(buf, path); // 复制路径到 buf
    p = buf + strlen(buf); // 将 p 指向 buf 的结尾
    *p++ = '/'; // 在路径后面添加 '/'

    while (read(fd, &de, sizeof(de)) == sizeof(de)) { // 读取目录项
        // 跳过 . 和 ..
        if (de.inum == 0 || !strcmp(de.name, ".") || !strcmp(de.name, ".."))
            continue; // 继续下一个循环

        // 将文件/目录名拼接到 buf 中
        memmove(p, de.name, DIRSIZ); // 复制文件/目录名到 buf
        p[DIRSIZ] = 0; // 添加字符串结尾符 '\0'

        // 获取文件状态
        if (stat(buf, &st) < 0) {
            fprintf(2, "find: cannot stat %s\n", buf); // 打印错误信息
            continue; // 继续下一个循环
        }

        // 如果是目录,递归调用 find 函数
        if (st.type == T_DIR) {
            find(buf, filename); // 递归调用 find 函数
        }
        // 如果是文件,检查文件名是否匹配
        else if (st.type == T_FILE && !strcmp(de.name, filename)) {
            printf("%s/%s\n", path, filename); // 打印匹配的文件路径
        }
    }

    close(fd); // 关闭目录文件描述符
}
