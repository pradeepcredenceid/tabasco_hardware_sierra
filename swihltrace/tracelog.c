#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/inotify.h>
#include <sys/types.h>
#include <sys/stat.h>

/* try ttyACM1 for 2G trace */
/* try ttyACM2 for 3G trace */

#define TRACE_PORT_PREFIX "/dev/"
#define TRACE_PORT_NAME "ttyACM1"
#define BAUDRATE	B115200
#define TRACE_FILE "/data/tracelog-%s.bin"

#define DEBUG 1
#define BUF_LEN 1024

static void usage(char *s)
{
    fprintf(stderr, " Logging with HL Modem Series: \n \
    %s [-d device] [-o logfile]: \n \n \
    OPTIONS: \n \
       -d device     - (Optional) /dev/ttyACMx port for HL firmware logging \n \
       -o logfile    - (Optional) fully qualified HL firmware log (output) file name \n ", s);   
}

static int open_tracelog_device(const char *devname)
{
	int h, retval;
	struct termios t;

	/* open device */
	if ((h = open(devname, O_RDWR)) == -1) {
		perror("open");
		return -1;
	}
	
	if ((retval = tcgetattr(h, &t)) < 0) {
		perror("tcgetattr");
		return -1;
	}
	
	cfsetispeed(&t, (speed_t)BAUDRATE);
	cfsetospeed(&t, (speed_t)BAUDRATE);

	t.c_cflag |= (CLOCAL | CREAD | CS8 | HUPCL);
	t.c_cflag &= ~(PARENB | CSIZE | CSTOPB);
	t.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	t.c_iflag &= ~(INLCR | ICRNL | IGNCR | IXON | IXOFF);
	t.c_iflag |= IGNPAR;
	t.c_oflag &= ~(OPOST | ONLRET | ONOCR | OCRNL | ONLCR);

	if ((retval = tcsetattr(h, TCSANOW, &t)) < 0) {
		perror("tcsetattr");
		return -1;
	}
	return h;			
}

static inline int close_tracelog_device(int h)
{
	return close(h);
}

/* return the bytes number of reading */
static int read_tracelog_device(int h, char *buf, int len)
{
	ssize_t ret;
	int read_len = len;

	while (len != 0 && (ret = read(h, (void *)buf, len)) != 0) {
		if (ret == -1) {
		 	if (errno == EINTR)
				continue;
			perror("read");
			return ret;
		}
		len -= ret;
		buf += ret;
	}
	return read_len - len;
}

static int open_tracelog_file(const char *filename)
{
	int retval;

	if ((retval = open(filename, O_WRONLY|O_CREAT|O_TRUNC, 0644)) == -1) {
		perror("open tracelog");
	}

	return retval;	
}

static int write_tracelog_file(int fd, char *buf, int len)
{
	int i, retval;
	int write_len = len;
#if 0
	for (i = 0; i < len; i++) {
		printf("%x ", (unsigned char)buf[i]);
		if (!(i % 16))
			printf("\n");
	}
#endif
	while (len != 0 && (retval = write(fd, buf, len)) != 0) {
		if (retval == -1) {
			if (errno == EINTR)
				continue;
			perror("write trace file");
			return retval;
		}
		len -= retval;
		buf += retval;	
	}
	return write_len - len;
}


static int close_tracelog_file(int fd)
{
	return close(fd);
}

int main(int argc, char **argv)
{
    unsigned int total_len;
    int h, fd, ret;
    int c;
    char buf[BUF_LEN];
    char filename[128];    
    char devicename[64];
    char *logfile = NULL;
    char *device = NULL;

#if 0	
	printf("wait for /dev/%s\n", TRACE_PORT_NAME);
	wait_for_tracelog_device(TRACE_PORT_NAME);
	/* wait device become stable */
	sleep(2);
#endif

    if (argc > 1) {
        if ((strncmp(argv[1], "-", 1)) != 0) {
            usage(argv[0]);
            exit(0);
        }
    }

    while((c = getopt(argc, argv, "?hd:o:"))!= EOF){
        switch (c)  
        {         
        case 'd':  
            device = optarg;  
            printf("device: %s\n", device);
            break;  
        case 'o':  
            logfile = optarg; 
            printf("logfile: %s\n", logfile);
            break; 
        case '?':
        case 'h':
        default:
            usage(argv[0]);
            exit(0);
        }   
    }

    if(device == NULL) {
        strcpy(devicename, TRACE_PORT_PREFIX);
        strcat(devicename, TRACE_PORT_NAME);
        device = devicename;
    }

    h = open_tracelog_device(device);
    if (h < 0) {
        printf("open tracelog device failed\n");
        goto exit_3;
    }

    if(logfile == NULL) {
        sprintf(filename, TRACE_FILE, TRACE_PORT_NAME);
        printf("trace file name: %s\n", filename);
    }
    else {
        strcpy(filename,logfile);
    }

    fd = open_tracelog_file(filename);
    if (fd < 0) {
        printf("open tracelog failed\n");
        goto exit_2;
    }

	printf("please AT+TRACE=1 enable trace function\n");
	total_len = 0;
	while (1) {
		ret = read_tracelog_device(h, buf, BUF_LEN);
		if (ret == -1) {
			printf("read tracelog device failed\n");
			break;
		}
		if (ret == 0) {
			printf("no trace data available, reading again\n");
		}
		
		ret = write_tracelog_file(fd, buf, ret);
		if (ret == -1) {
			printf("write tracelog file failed\n");
			break;
		}
		total_len += ret;
		printf("written: %d bytes to %s\n", total_len, filename); 
	}

exit_1:
	close_tracelog_file(fd);
exit_2:
	close_tracelog_device(h);
exit_3:
	return -1;
}

