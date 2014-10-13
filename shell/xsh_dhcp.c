#include <xinu.h>


command xsh_dhcp(int nargs, char *args[]){
	sendDiscoverPacket();
	return OK;
}
