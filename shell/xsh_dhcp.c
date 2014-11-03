#include <xinu.h>


command xsh_dhcp(int nargs, char *args[]){
	//this is the flag declared in netdaemon that knows if we are looking to update our ip
	ipSet = FALSE;

	//send discover packet, function in packet.c
	sendDiscoverPacket();
	return OK;
}
