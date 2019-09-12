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

int main(int argc, char **argv)
{
	int fd;

	char interface_name[] = "tun0";

	fd = tun_alloc(interface_name);

	while(1) {
		sleep(1);
	}

	return 0;
}
