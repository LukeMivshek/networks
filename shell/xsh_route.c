#include <xinu.h>

int parseIP(char*, uchar[]);
int parse(char[]);
uchar* parseCIDR_Mask(char*);
void displayRoutingTable(void);
void deleteRoutingTableEntry(uchar[]);
void addRoutingTableEntry(uchar[]);
uchar destIP[IP_ADDR_LEN];
uchar gtwyIP[IP_ADDR_LEN];
uchar* destIpPtr;
uchar* gtwyIpPtr;
char* destArgPtr;
char* gtwyArgPtr;


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
		printf("Add Routing entry\n");
		destArgPtr = args[2];
		gtwyArgPtr = args[3];
		if(parseIP(destArgPtr, destIpPtr) && parseIP(gtwyArgPtr, gtwyIpPtr)){
			routeAdd(destIP, parseCIDR_Mask(destArgPtr), gtwyIP, parse(args[4]));
		}else{
			
		}
	}else if(nargs == 3 && (!strcmp(args[1], "delete"))){
		printf("Delete Routing entry\n");
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
	printf("Entry   Destination     Netmask         Gateway       Interface\n");
	for(v = 0; v < ROUTEENT_LEN; v++){
		printf("%d       ", v);
		int q = 0;
		for(q = 0; q < IP_ADDR_LEN; q++){
			printf("%02X ", routeTab.routes[v].destNetwork[q]);
		}
		printf("    ");
		for(q = 0; q < IP_ADDR_LEN; q++){
			printf("%02X ", routeTab.routes[v].netmask[q]);
		}
		printf("    ");
		for(q = 0; q < IP_ADDR_LEN; q++){
			printf("%02X ", routeTab.routes[v].gateway[q]);
		}
		printf("  ");
		
		if(routeTab.routes[v].interface == 2){
			printf("ETH0");
		}else if(routeTab.routes[v].interface == -1){
		}

		//else if(routeTab.routes[v].interface == 1){
		//	printf("ETH1");
		//}
		else{
			printf("UNKNOWN");
		}

		printf("\n");
	}
}

void deleteRoutingTableEntry(uchar destNetwork[])
{
	
}

