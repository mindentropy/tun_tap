#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/if_tun.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#define MTU 	1500U
#define PORT 	55555

int tun_alloc(char *dev)
{
	int fd, err;
	struct ifreq ifr;

	fd = open("/dev/net/tun", O_RDWR);

	if(fd < 0) {
		perror("tun_test");
		return 0;
	}
	
	memset(&ifr, 0, sizeof(ifr));
	
	/* Set flags TUN or TAP etc */
	ifr.ifr_ifru.ifru_flags = IFF_TUN | IFF_NO_PI;

	/* Do a string copy of the interface name */
	if(*dev) {
		strncpy(ifr.ifr_ifrn.ifrn_name, dev, IFNAMSIZ);
	}

	err = ioctl(fd, TUNSETIFF, (void *) &ifr);

	if(err < 0) {
		perror("ioctl(TUNSETIFF)");
		close(fd);
		return err;
	}

	strcpy(dev, ifr.ifr_ifrn.ifrn_name);

	return fd;
}

int dump_bytes(unsigned char *buffer, int len)
{
	int i = 0;

	for(i = 0; i<len; i++) {
		fprintf(stdout, "0x%02x ", buffer[i]);
	}

	fprintf(stdout,"\n");
}

int main(int argc, char **argv)
{
	int tun_fd, sock_fd;
	int ret;
	int nread;
	struct sockaddr_in remote;

	char interface_name[] = "tun0";
	char remote_ip[] = "193.0.0.1";
	char buffer[MTU];
	
	tun_fd = tun_alloc(interface_name);
	
	if(tun_fd != -1) {
		fprintf(stdout, "Tun alloc successful\n");
	}

	sock_fd = socket(AF_INET, SOCK_STREAM, 0);

	if(sock_fd < 0) {
		perror("socket creation");
		exit(1);
	}

	memset(&remote, 0, sizeof(struct sockaddr_in));

	remote.sin_family = AF_INET;
	remote.sin_addr.s_addr = inet_addr(remote_ip);
	remote.sin_port = htons(PORT);

	ret = connect(sock_fd, (struct sockaddr *) &remote, sizeof(remote));

	if(ret != 0) {
		perror("connect()");
		exit(1);
	}

	fprintf(stdout, "Client connected to server");


	return 0;
}
