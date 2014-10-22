#include <xinu.h>


command xsh_dhcp(int nargs, char *args[]){
	ipSet = FALSE;
	sendDiscoverPacket();
	return OK;
}
