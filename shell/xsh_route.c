#include <xinu.h>

int parseIP(char*, uchar[]);
int parse(char[]);
int parseCIDR_Mask(char*);
void displayRoutingTable(void);
void deleteRoutingTableEntry(uchar[]);
void addRoutingTableEntry(uchar[]);
uchar destIP[IP_ADDR_LEN];
uchar gtwyIP[IP_ADDR_LEN];
uchar* destIpPtr;
uchar* gtwyIpPtr;
char* destArgPtr;
char* gtwyArgPtr;
void printDecimalIP(uchar[]);

command xsh_route(int nargs, char *args[]){
	bzero(destIP, IP_ADDR_LEN);
	bzero(gtwyIP, IP_ADDR_LEN);	
	destIpPtr = &destIP[0];
	gtwyIpPtr = &gtwyIP[0];

	//view table command
	if(nargs == 2 && (!strcmp(args[1], "view"))){
		printf("Routing Table\n");
		displayRoutingTable();
	}else if(nargs == 5 && (!strcmp(args[1], "add"))){
		printf("Adding Routing entry\n");
		destArgPtr = args[2];
		gtwyArgPtr = args[3];
		if(parseIP(destArgPtr, destIpPtr) && parseIP(gtwyArgPtr, gtwyIpPtr)){
			routeAdd(destIP, parseCIDR_Mask(destArgPtr), gtwyIP, parse(args[4]));
		}else{
			
		}
	}else if(nargs == 3 && (!strcmp(args[1], "delete"))){
		printf("Deleting Routing entry\n");
		destArgPtr = args[2];
		parseIP(destArgPtr, destIpPtr);
		deleteRoutingTableEntry(destIP);
	}else{
		printf("Invalid Routing input\n");
	}

	return OK;	
}

/*
 * print the routing table
 */
void displayRoutingTable(){
	int v = 0; 
	printf("%-8s%-15s\t%-15s\t%-15s\t%-15s\n", "Entry", "Destination", "Netmask", "Gateway", "Interface");
	while(routeTab.routes[v].interface != -1){
		if(v == ROUTEENT_LEN){
			break;
		}

		printf("%d\t", v);
		printDecimalIP(routeTab.routes[v].destNetwork);
		printf("\t");
		printDecimalIP(routeTab.routes[v].netmask);
		printf("\t");
		printDecimalIP(routeTab.routes[v].gateway);
		printf("\t");
		
		if(routeTab.routes[v].interface == 2){
			printf("ETH0");
		}else{
			printf("Unknown");
		}
		//next entry
		printf("\n");

		v = v +1;
	}
}

/*
 * takes a destination IP address and clears the matching entry if it exists
 */
void deleteRoutingTableEntry(uchar destNetwork[]){
	int w = 0; 

	//we wont ever delete the default entry here
	for(w = 0; w < ROUTEENT_LEN-1; w++){
		
		bool mismatch = FALSE;

		int u = 0;
		for(u = 0; u < IP_ADDR_LEN; u++){
			if(destNetwork[u] != routeTab.routes[w].destNetwork[u]){
				mismatch = TRUE;
				break;
			}
		}

		if(!mismatch){
			//clear all the feilds of the entry, set interface back to -1 (free)
			int c = 0;
			for(c = 0; c < IP_ADDR_LEN; c++){
				routeTab.routes[w].destNetwork[c] = 0x00;
				routeTab.routes[w].netmask[c] = 0x00;
				routeTab.routes[w].gateway[c] = 0x00;
			}
			routeTab.routes[w].interface = -1;

			printf("Entry %d cleared from routing table\n", w);
			return;
		}
	}
	
	printf("Destination network not found in routing table\n");
	return;
}
