#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/input.h> // this does not compile
#include <errno.h>

#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

 

//去除尾部空格
char *rtrim(char *str);
char *rtrim(char *str)
{
    if (str == NULL || *str == '\0')
    {
        return str;
    }
 
    int len = strlen(str);
    char *p = str + len - 1;
    while (p >= str  && isspace(*p))
    {
        *p = '\0';
        --p;
    }
 
    return str;
}
 
//去除首部空格
char *ltrim(char *str);
char *ltrim(char *str)
{
    if (str == NULL || *str == '\0')
    {
        return str;
    }
 
    int len = 0;
    char *p = str;
    while (*p != '\0' && isspace(*p))
    {
        ++p;
        ++len;
    }
 
    memmove(str, p, strlen(str) - len + 1);
 
    return str;
}
 
//去除首尾空格
char *trim(char *str);
char *trim(char *str)
{
    str = rtrim(str);
    str = ltrim(str);
    
    return str;
}

int main(int argc, char *argv[])
{
    printf("argv[1]: %s\n", argv[1]);

    int fd;
    ssize_t ret;
    int version;
    struct input_event event;
 
    if(argc < 3) {
        fprintf(stderr, "use: %s [device (type code value)file]\n", argv[0]);
        return 1;
    }
 
    fd = open(argv[1], O_RDWR);
    if(fd < 0) {
        fprintf(stderr, "could not open %s, %s\n", argv[1], strerror(errno));
        return 1;
    }
    if (ioctl(fd, EVIOCGVERSION, &version)) {
        fprintf(stderr, "could not get driver version for %s, %s\n", argv[1], strerror(errno));
        return 1;
    }

    FILE *fp = fopen(argv[2], "r");
    if (fp != NULL) {

        char CmdLine[128];
        while(fscanf(fp,"%[^\n]", CmdLine)!=EOF) {
            fgetc(fp);
            printf("line: %s\n", CmdLine);

            char *str = strdup(CmdLine);
            char *string = trim(str);

            char *values[3];
            int i = 0;
            char *val;
            while( (val = strsep((char **)&string, " ")) != NULL) {
                // printf("E: %s\n", val);
                values[i] = val;
                i++;
                if (i >= sizeof(values)){
                    break;
                }
            }

            free(str);

            printf("values[0]: %s\n", values[0]);
            printf("values[1]: %s\n", values[1]);
            printf("values[2]: %s\n", values[2]);

            memset(&event, 0, sizeof(event));
            event.type = atoi(values[0]);
            event.code = atoi(values[1]);
            event.value = atoi(values[2]);
            ret = write(fd, &event, sizeof(event));
            if(ret < (ssize_t) sizeof(event)) {
                fprintf(stderr, "write event failed, %s\n", strerror(errno));
                return -1;
            }

            struct timespec ts, ts1;
            if (argc == 4) {
                ts.tv_nsec = atol(argv[3]);
            } else {
                ts.tv_nsec = 10000000; 
            }
            ts.tv_sec = 0;
            if ( nanosleep(&ts, &ts1) == -1 ) {
                printf("nanosleep error!\n");
                return -1;
            }

        }

    }

    return 0;
}