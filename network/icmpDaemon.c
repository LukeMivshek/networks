/**
 * Pings echo requets are sent from the xsh_ping shell, utilizing the packet sends in packet.c
 * Responses from our echo requests are handled here
 * In icmpDaemon, we also utilize the packet sends in packet.c to echo requests
 * with echo replys
 *
 * Casey French & Luke Mivshek
 */

#include <xinu.h>

struct packet *icmp_pktPointer;
struct packet icmp_pkt;

void icmpDaemon(){
	printf("Started icmpDaemon\n");
	
	while(1){
		//recieve icmp type packets fro netDaemon
		message msg = receive();
			
		printf("ICMP packet recieved in icmpDaemon\n");
		
		icmp_pktPointer = (struct packet*)msg;
		icmp_pkt = *icmp_pktPointer;

		free ((void *) msg);
	}
}
