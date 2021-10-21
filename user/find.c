#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char *path, char *file_name) {
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;
  if((fd = open(path, 0)) < 0) {
      fprintf(2,"find: cannot open %s\n", path);
      exit(1);
  }
  if(fstat(fd, &st) < 0) {
      fprintf(2, "find: cannot stat %s\n", path);
      close(fd);
      exit(1);
  }
  if(st.type == T_FILE) {
    printf("cannot find file in file\n");
    close(fd);
    exit(1);
  }
  else if(st.type == T_DIR) { //path为文件夹后获取其结构，对每个文件信息筛查
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("find: path too long\n");
      close(fd);
      exit(1);
    }
    //提前做好前置路径
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      printf("inum=%d, name=%s\n", de.inum, de.name);
      if(de.inum == 0 || !strcmp(de.name, "..") || !strcmp(de.name, "."))  //忽略掉.和..
      // if(de.inum == 0 || de.inum == 1)  
        continue;
      // printf("inum=%d, name=%s\n", de.inum, de.name);
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;        //得到path文件夹下的文件或文件夹路径
      // printf("remember: search %s\n", buf);
      if (stat(buf, &st) < 0) {
        printf("find: cannot stat %s\n", buf);
        continue;
      }
      if(st.type == T_FILE && !strcmp(de.name, file_name)) {
          printf("%s\n", buf);
      } else if(st.type == T_DIR) {
        // printf("-inum=%d, name=%s\n", de.inum, de.name);
        find(buf, file_name);
      }
    }
  }
  close(fd);
}

void main(int argc, char *argv[]) {
  if(argc != 3) {
    printf("find: wrong parameter!\n");
    exit(1);
  }
  find(argv[1], argv[2]);
  exit(0);
}