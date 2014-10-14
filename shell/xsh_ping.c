/**
 * This command handles sending echo requets. icmpDaemon handles replys and prints
 * If there is no third argument for number of requets to be sent, we send 10
 *
 * Casey French & Luke Mivshek
 */
#include <xinu.h>

void sendPings(uchar[], int);
int getEntry(uchar[]);
char* ipPtr;
int parseIP(char*, uchar *);
uchar pingIPAddress[IP_ADDR_LEN];
bool existsInTable(uchar[]);

command xsh_ping(int nargs, char *args[]){
	//zero out IP Address variable
	bzero(pingIPAddress, IP_ADDR_LEN);

	//setting the default value of pings
	int numOfPings = 10;

	//two args
	if(nargs == 2){
		ipPtr = args[1];
			
		if(!parseIP(ipPtr, pingIPAddress)){
			printf("Error parsing IP\n");
			return OK;
		}
		printf("calling sendPing from two args with %d number of pings\n", numOfPings);
		sendPings(pingIPAddress, numOfPings);

	//a number is given, use that as number of pings
	}else if(nargs == 3){
			numOfPings = 0;
			char *pings = args[2];
			while(*pings){
				if(isdigit((uchar)*pings)){
					numOfPings *= 10;
					numOfPings += *pings - '0';	
				}else{
					printf("Incorrect 3rd argument of ping, should be a numerical value\n");
				}
				pings++;
			}
			printf("calling sendPing from three args with %d number of pings\n", numOfPings);
			sendPings(pingIPAddress, numOfPings);

	}else{
		printf("Incorrect number of args\n");
	}

	printf("Returning OK in xsh_ping (shell)\n");
	return OK;
}
/*
 * send a ping
 */
void sendPings(uchar ipAddr[], int numOfPings){

	//look up the index in the table with getEntry
	int index = getEntry(ipAddr);
	
	if(index == -1){
		printf("IP requested for Ping could not be resolved.\n");
		return;
	}	

	int q = 0;
	for(q = 0; q < numOfPings; q++){
		//send one packet then sleep
		printf("Sending echo request with index %d\n", index);
		
		sendEchoRequestPacket(arptab.arps[index].macAddress, arptab.arps[index].ipAddress, currpid, (numOfPings-q-1));
		sleep(1000);
	}

	printf("Done sending ping requests\n");
}

/*
 * Return the entry number  of our IP Address in the arptab
 */
int getEntry(uchar ipAddr[]){
	bool mismatch = FALSE;
	if(existsInTable(ipAddr)){

		int a = 0;
		while(a < ARPENT_LEN){ 
			//if it is not in the empty state, compare IP Addresses
			if(arptab.arps[a].state != ARPENT_STATE_EMPTY){
				int u = 0;
				for(u = 0; u < IP_ADDR_LEN; u++){
					if(arptab.arps[a].ipAddress[u] != ipAddr[u]){
						mismatch = TRUE;
						break;
					}
				}
			
				//no mismatch was found
				if(!mismatch){
					printf("getMac returning: ");
					int w = 0;
					for(w = 0; w < ETH_ADDR_LEN; w++){
						printf("%02X ", arptab.arps[a].macAddress[w]);
					}
					printf("\n");
					return a;	
				}
			//its in the free state, skip it
			}else{
				a++;
			}
		}
	}

	printf("Returning SYSERR in getMac, no entry found based on IP Address\n");
	return SYSERR;	
	
}


