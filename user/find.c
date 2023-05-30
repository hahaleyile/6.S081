#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void recurs_find(char *path, char *name)
{
    struct dirent de;
    struct stat st;

    int inode;
    if ((inode = open(path, 0)) < 0)
    {
        printf("find: error open path '%s'\n", path);
        return;
    }

    char str[512];
    strcpy(str, path);
    int len = strlen(str);
    str[len++] = '/';
    str[len] = 0;
    while (read(inode, &de, sizeof(de)) == sizeof(de))
    {
        // 8.11: Directory entries with inode number zero are free
        if (de.inum == 0)
            continue;

        if (!strcmp(de.name, ".") || !strcmp(de.name, ".."))
        {
            continue;
        }

        if (!strcmp(de.name, name))
        {
            printf("%s/%s\n", path, de.name);
        }

        memmove(str + len, de.name, DIRSIZ);
        str[len + DIRSIZ] = 0;
        if (stat(str, &st) < 0)
        {
            printf("path: cannot stat '%s'\n", str);
            continue;
        }

        if (st.type == T_DIR)
        {
            recurs_find(str, name);
        }
    }

    close(inode);
}

void find(char *path, char *name)
{
    struct stat st;
    int fd;

    if ((fd = open(path, 0)) < 0)
    {
        printf("find: no such path '%s'\n", path);
        return;
    }

    if (fstat(fd, &st) < 0)
    {
        printf("find: cannot stat '%s'\n", path);
        goto Close;
    }

    if (st.type != T_DIR)
    {
        printf("usage: find <DIRECTORY> <filename>\n");
        goto Close;
    }

    recurs_find(path, name);

Close:
    close(fd);
    return;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        exit(1);
    }
    find(argv[1], argv[2]);

    exit(0);
}