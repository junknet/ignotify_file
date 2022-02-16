#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>

#define NAME_MAX 1024
#define LOGA printf
#define LOGB printf
const int buflen = sizeof(struct inotify_event) * 0x100;

//监控内存是否被修改事件
int main() {
  char dirName[NAME_MAX] = {0};
  //用于监控/proc/pid/maps的数据
  snprintf(dirName, NAME_MAX, "/proc/%d/maps", getpid());

  int fd = inotify_init();
  if (fd < 0) {
    LOGA("inotify_init err.\n");
    return;
  }

  int wd = inotify_add_watch(fd, dirName, IN_ALL_EVENTS);
  if (wd < 0) {
    LOGA("inotify_add_watch err.\n");
    close(fd);

    return;
  }

  char buf[buflen];
  fd_set readfds;

  while (1) {

    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);

    int iRet = select(fd + 1, &readfds, 0, 0, 0); // 此处阻塞
    LOGB("iRet的返回值:%d\n", iRet);
    if (-1 == iRet)
      break;

    if (iRet) {

      memset(buf, 0, buflen);
      int len = read(fd, buf, buflen);

      int i = 0;

      while (i < len) {

        struct inotify_event *event = (struct inotify_event *)&buf[i];

        LOGB("1 event mask的数值为:%d\n", event->mask);

        if ((event->mask == IN_OPEN)) {

          // 此处判定为有true,执行崩溃.

          LOGB("2 有人打开pagemap,第%d次.\n\n", i);
        }
        i += sizeof(struct inotify_event) + event->len;
      }
    }
  }

  inotify_rm_watch(fd, wd);
  close(fd);

  return 0;
}
