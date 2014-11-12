#include <xinu.h>

int getEntry(uchar[]);

void routeForward(uchar packet[], uchar check[]){

	uchar *mac;
	mac = arptab.arps[getEntry(check)].macAddress;
	

	struct  ethergram *route_ethergram = (struct ethergram*)packet;
	int y;
	for(y = 0; y < ETH_ADDR_LEN; y++){
		route_ethergram->dst[y] = mac[y];
	}

	write(ETH0, packet, PKTSZ);
	kill(currpid);
}

