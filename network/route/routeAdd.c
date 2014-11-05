#include <xinu.h>

void printDecimalIP(uchar[]);
void printIP(uchar[]);
int getNextEntry(void);
bool existsInTable(uchar[]);

/*
 * route table entry 0 is the default file, use current entry field of routeTab as the index
 * to update table
 */
void routeAdd(uchar *destIP, int ones, uchar *gtwyIP, int interface){

	int entry = getNextEntry();

	if(entry == -1){
		printf("No free table slots, cannot update route table\n");
		return;
	}
	
	if(ones < 0 || ones > 32){
		printf("Invalid number of bits for mask\n");
		return;
	}

	uchar gateway[IP_ADDR_LEN];
	memcpy(&gateway, gtwyIP, IP_ADDR_LEN);

	//if an arp is unsuccessful, return
	if(existsInTable(gateway)){
		printf("resolved mac\n");
	}else{
		printf("unresolved mac, cannot add route\n");
		return;
	}

	int mask = 0xFFFFFFFF << (sizeof(int)*8 - ones);

	routeTab.routes[entry].netmask[0] = (uchar)((mask >> 24) & 0xFF);
	routeTab.routes[entry].netmask[1] = (uchar)((mask >> 16) & 0xFF);
	routeTab.routes[entry].netmask[2] = (uchar)((mask >> 8) & 0xFF);
	routeTab.routes[entry].netmask[3] = (uchar)((mask >> 0) & 0xFF);

	//copy addresses	
	memcpy(&routeTab.routes[entry].destNetwork, destIP, IP_ADDR_LEN);
	memcpy(&routeTab.routes[entry].gateway, gtwyIP, IP_ADDR_LEN);

	//printIP(routeTab.routes[entry].destNetwork);
	//printIP(routeTab.routes[entry].gateway);
	printIP(routeTab.routes[entry].netmask);
	
	routeTab.routes[entry].interface = interface;

	printf("Route table entry %d updated\n", entry);

	return;
}

/*
 * return the index of a free slot in the route table
 * by route entry interface feilds, we don't touch the 0 default entry
 * -1 indicates no free slots
 */
int getNextEntry(){

	int d = 1;
	for(d = 1; d < ROUTEENT_LEN; d++){
		if(routeTab.routes[d].interface == -1){
			return d; 
		}
	}
	
	//if we have not returned yet, nothing is in the 'free' state
	return -1;
}
