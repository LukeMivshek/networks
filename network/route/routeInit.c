#include <xinu.h>

void buildRoutingTable(void);

/*
 * initialize routing, build the table
 */
void routeInit(){
	buildRoutingTable();

	//set default gateway interface to ETH0
	routeTab.routes[0].interface = 2;
}

void buildRoutingTable(void)
{
	int s = 0; 

	for(s = 0; s < ROUTEENT_LEN; s++){
		//zero destNetwork, netmask and gateway
		int w = 0;
		for(w = 0; w < IP_ADDR_LEN; w++){
			routeTab.routes[s].destNetwork[w] = 0x00;
			routeTab.routes[s].netmask[w] = 0x00;
			routeTab.routes[s].gateway[w] = 0x00;
		}

		//default interface is ETH0
		routeTab.routes[s].interface = -1;

	}
}
