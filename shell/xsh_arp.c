#include <xinu.h>

void printTable(void);
void printUsage(char*);
bool existsInTable(uchar[]);
int arpResolve(uchar*, uchar*);
int parseIP(char*, uchar *);
void removeFromTable(uchar[]);
int isInTable(uchar[]);
uchar ipAddress[IP_ADDR_LEN];
char* ipPointer;
int getValidTableLocation(void);

command xsh_arp(int nargs, char *args[]){
	
	//local variable to store the incoming ip address
	bzero(ipAddress, IP_ADDR_LEN);
	//ipPointer = &ipAddress[0];

	//must be at least one to have called the arp shell command
	if(nargs == 3){
		
		//get ip from args array
		ipPointer = args[2];
		
		if(!parseIP(ipPointer, ipAddress)){
			printf("Error parsing IP.\n");	
			return OK;
		}

		if(!strcmp(args[1],"request")){	
			printf("Request A New Mapping for IP Address: ");
			int j = 0; 
			
			for(j = 0; j < IP_ADDR_LEN; j++){
				printf("%02X ", ipAddress[j]);
			}	
			printf("\n");

			if(existsInTable(ipAddress) == TRUE){
				return OK;
			}else{
				return SYSERR;
			}
			
		}else if(!strcmp(args[1],"remove")){
			//printf("Eliminate Existing Mapping of IP Address, %s\n", ipAddress);
			removeFromTable(ipAddress);	
		}
		
		else{
			printf("Incorrect Number of args\n");
			printUsage(args[0]);
		}

	}else if(nargs == 2){
		//decide what to do based on shell args	
		if(!strcmp(args[1],"help")){
			printUsage(args[0]);
			return OK;
		}else if(!strcmp(args[1], "table")){
			printTable();	
		}else{
			printf("Invalid arp arg\n");
			printUsage(args[0]);
		}
	}else{
		printf("Incorrect number of args\n");
		printUsage(args[0]);
	}

	return OK;
}

/*
 * print usage for arp shell command
 */
void printUsage(char *prgrm){
	
	printf("usage: ");
	printf("%s",prgrm);
	printf(" arp_function optional_IP_Address (4 bytes)\n");
	printf("\tarp_functions:\n");
	printf("\t\ttable - Display Current Table\n");
	printf("\t\tnewmap - Request a New Mapping\n");
	printf("\t\tremovemap - Eliminate Existing Mapping\n\n");
}

/*
 * takes an ip address and returns TRUE if it exists in the arp table
 */
bool existsInTable(uchar ipAddr[]){
	printf("Checking if IP Address exists in table.\n");
	
	//uchar pointers to pass into arpResolve	
	uchar* ip;
	uchar* mac;
	int k = 0;
	//printf("Arpnum = %d", arptab.arpnum);
	//check if ip is in the table
	if(isInTable(ipAddr) == -1){
		printf("IP address not in table\n");
		printf("Attempting to resolve requested mapping...\n");
		
		ip = (uchar*)&arptab.arps[arptab.arpnum].ipAddress;
		mac = (uchar*)&arptab.arps[arptab.arpnum].macAddress;
	
		ip = ipAddr;
	
		//setting state to unresolved
		
		arptab.arpnum = getValidTableLocation();

		arptab.arps[arptab.arpnum].state = ARPENT_STATE_UNRESOLVED;		

		printf("Modifying entry: %d\n", arptab.arpnum);

		if(arpResolve(ip, mac) == OK){
			printf("Mapping resolved\n");
			//setting state to resolved
			
			arptab.arps[arptab.arpnum].state = ARPENT_STATE_RESOLVED;
			
			return TRUE;
		}else{
			printf("Resolve failed.\n");
			//setting state to empty and reset counter
			arptab.arps[arptab.arpnum].state = ARPENT_STATE_EMPTY;
		}
		return FALSE;
	}else{
		printf("IP Address found in arp table, returning true in existsInTable\n");
		return TRUE;
	}
	
	//we should have returned true or false 
	printf("Returning SYSERR in existsInTable function\n");
	return SYSERR;
}

/*
 * function prints entire contents of arp table
 */
void printTable(){
	printf("Number of Entries: %d\n", arptab.arpnum);
	printf("---- ARP Table Start ----\n");
	int m = 0;
	for(m = 0; m < ARPENT_LEN; m++){
		printf("Entry %d: ", m);
		printf("IP: ");
		int o;
		for(o = 0; o < IP_ADDR_LEN; o++){
			printf("%02X ", arptab.arps[m].ipAddress[o]);
		} 
		printf("MAC: ");
		for(o = 0; o < ETH_ADDR_LEN; o++){
			printf("%02X ", arptab.arps[m].macAddress[o]);
		}
		printf("State: ");		
		if(arptab.arps[m].state == 0){
			printf("EMPTY ");
		}else if(arptab.arps[m].state == 1){
			printf("RESOLVED ");
		}else if(arptab.arps[m].state == 2){
			printf("UNRESOLVED ");
		}else{
			printf("UNKNOWN, value: %d ", arptab.arps[m].state);
		}

		printf("Age: %d \n", arptab.arps[m].age);
		//printf("PID: %d \n", arptab.arps[m].age);
	}
	printf("---- ARP Table End ----\n");
}

/*
 * Return true if the IP was found and removed, false if it was not found
 * For the age, we first save the age, decrement every age above the saved age
 * and set the age of the entry to 9
 */
void removeFromTable(uchar ipToRemove[]){
	
	int currentAge = 0;

	int index = isInTable(ipToRemove);
	
	if(index == -1){
		printf("IP not in table.\n");
	}else{
		printf("Removing IP from table: ");
		int n = 0; 
		for(n = 0; n < IP_ADDR_LEN; n++){
			printf("%02X ", ipToRemove[n]);
		}
		printf("\n");

		bzero(arptab.arps[index].ipAddress, IP_ADDR_LEN);
		bzero(arptab.arps[index].macAddress, ETH_ADDR_LEN);
		arptab.arps[index].state = ARPENT_STATE_EMPTY;
		
		currentAge = arptab.arps[index].age;

		arptab.arps[index].pid = 0;
	}

	int x = 0;	
	for(x = 0; x < ARPENT_LEN; x++){
		if(arptab.arps[x].age > currentAge){
			arptab.arps[x].age = arptab.arps[x].age - 1;
		}
	}
	
	arptab.arps[index].age = 9;
}


int isInTable(uchar ipAddr[]){
	int k = 0;

	uchar *ip;
	uchar *mac;

	//loop through table and compare IP Addresses
	for(k = 0; k < ARPENT_LEN; k++){
		//printf("IP Compare | ipAddr: %02X arptab.arps[k].ipAddress: %02X\n", ipAddr, arptab.arps[k].ipAddress);

		bool mismatch = FALSE;

		int z = 0;
		for(z = 0; z < IP_ADDR_LEN; z++){
			if(ipAddr[z] == arptab.arps[k].ipAddress[z]){
				//printf("ipAddr: %02X ipAddress: %02X\n", ipAddr[z], arptab.arps[k].ipAddress[z]);
			}else{
				//printf("Unequal IP Addresses\n");
				mismatch = TRUE;
				//printf("mismatch = true\n");
				break;
			}
		}

		if(!mismatch){
			printf("Returing true in isInTable IP: %d MAC: %d\n", arptab.arps[k].ipAddress, arptab.arps[k].macAddress);
			return k;
		}
		
	}	
	
	printf("Returning SYSERR in isInTable(uchar ipAddr)\n");	
	return -1;
}

/*
 * First loop through table to find an empty state. 
 * If we don't find one and return, look for the age 9 entry and return
 */
int getValidTableLocation(void){
	int c = 0;
	for(c = 0; c < ARPENT_LEN; c++){
		if(arptab.arps[c].state == ARPENT_STATE_EMPTY){
			//printf("Returning %d in getValidTableLocation (empty state)\n");
			return c;
		}
	}

	for(c = 0; c < ARPENT_LEN; c++){
		if(arptab.arps[c].age == 9){
			printf("Returning %d in getValidTableLocation (age 9) \n");
			return c;
		}
	}

	printf("Returning SYSERR in getValidTableLocation\n");
	return SYSERR;
}
