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
	ifr.ifr_ifru.ifru_flags = IFF_TUN;

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

		if(i && ((i % 64) == 0)) {
			printf("\n");
		}

		fprintf(stdout, "0x%02x ", buffer[i]);
		fflush(stdout);
	}
	fprintf(stdout,"\n");
}

int main(int argc, char **argv)
{
	int tun_fd;
	int nread;

/*	struct sockaddr_in remote; */

	char interface_name[] = "tun0";
	char remote_ip[] = "193.0.0.1";
	char buffer[64];

	
	tun_fd = tun_alloc(interface_name);
	
	if(tun_fd != -1) {
		fprintf(stdout, "Tun alloc successfull\n");
	}

	while(1) {

		nread = read(tun_fd, buffer, sizeof(buffer));
		
		if(nread < 0) {
			close(tun_fd);
			exit(1);
		}
		
		dump_bytes(buffer, nread);
	}

/*	memset(&remote, 0, sizeof(struct sockaddr_in));

	remote.sin_family = AF_INET;
	remote.sin_addr.s_addr = inet_addr(remote_ip);*/

	return 0;
}
