/**
 * Pings echo requets are sent from the xsh_ping shell, utilizing the packet sends in packet.c
 * Responses from our echo requests are handled here
 * In icmpDaemon, we also utilize the packet sends in packet.c to echo requests
 * with echo replys
 *
 * Casey French & Luke Mivshek
 */

#include <xinu.h>

void icmpDaemon(){
	printf("Started icmpDaemon\n");
	
	while(1){
		//recieve icmp type packets fro netDaemon
		message msg = receive();
		
		printf("ICMP packet recieved in icmpDaemon\n");
		
	}
}
