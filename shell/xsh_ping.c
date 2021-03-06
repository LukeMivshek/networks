/**
 * This command handles sending echo requets. icmpDaemon handles replys and prints
 * If there is no third argument for number of requets to be sent, we send 10
 *
 * Casey French & Luke Mivshek
 */
#include <xinu.h>

void sendPings(uchar[], int, int);
int getEntry(uchar[]);
char* ipPtr;
int parseIP(char*, uchar *);
int parse(char[]);
uchar pingIPAddress[IP_ADDR_LEN];
bool existsInTable(uchar[]);

command xsh_ping(int nargs, char *args[]){
	//zero out IP Address variable
	bzero(pingIPAddress, IP_ADDR_LEN);

	//setting the default value of pings
	int numOfPings = 10;
	int size = MIN_ECHO_SIZE;

	//two args
	if(nargs == 2){
		ipPtr = args[1];
		if(!parseIP(ipPtr, pingIPAddress)){
			printf("Error parsing IP\n");
			return SYSERR;
		}
		
		//will be called using all defaults
		sendPings(pingIPAddress, numOfPings, size);

	//a number is given, use that as number of pings
	}else if(nargs == 3){
		ipPtr = args[1];
		if(!parseIP(ipPtr, pingIPAddress)){
			printf("Error parsing IP\n");
			return SYSERR;
		}
	
		numOfPings = parse(args[2]);

		//size is default
		sendPings(pingIPAddress, numOfPings, size);	

	}else if(nargs == 4){
		ipPtr = args[1];
		if(!parseIP(ipPtr, pingIPAddress)){
			printf("Error parsing IP\n");
			return SYSERR;
		}

		numOfPings = parse(args[2]);
		size = parse(args[3]);
		
		//both number of pings and size have been set from user
		sendPings(pingIPAddress, numOfPings, size);
	}else{
		printf("Incorrect number of args\n");
	}
	
	return OK;
}

/*
 * takes the target ip address, looks up its mac in table and resolves if necessary
 * takes number of pings and loops over a sendEchoRequests
 * waits for message from netDaemon and collects data, prints at the end
 */
void sendPings(uchar ipAddr[], int numOfPings, int size){
	printf("Sending %d pings,", numOfPings);
	printf(" %d bytes each\n", size);

	int time = 0;
	int transmit = 0;
	int drop = 0;
	int temp;
	//look up the index in the table with getEntry
	//int index = getEntry(ipAddr);
	
	int totalTime = clocktime;
	int q = 0;
	for(q = 0; q < numOfPings; q++){
		//send one packet then wait on reply (comes from netdaemon)
		printf("");
		time = clocktime;

		int routeIndex = routeNextHop(ipAddr);

		//sendEchoRequestPacket(arptab.arps[getEntry(ipAddr)].macAddress , ipAddr, currpid, (numOfPings-q-1), size);
		
		if(routeIndex == -1){
			sendEchoRequestPacket(arptab.arps[getEntry(ipAddr)].macAddress , ipAddr, currpid, (numOfPings-q-1), size);
		}else if(routeIndex == -2){
			printf("No route avaliable\n");
		}else{
			sendEchoRequestPacket(arptab.arps[getEntry(routeTab.routes[routeIndex].gateway)].macAddress , ipAddr, currpid, (numOfPings-q-1),size);
		}
		//wait on the response from net daemon
		message msg = recvtime(2000);
		if(msg < 0){
			temp = clocktime - time;
			printf("No response, time: %dms\n", temp);
			drop++;				
		}else{
			transmit++;
			sleep(1000);
			temp = clocktime - time;
			printf("time: %dms\n", temp);
		}
		int pckstrt = (int)msg;

	}
	temp = clocktime - totalTime;
	printf("Done sending ping requests\n");
	printf("-----Ping Statistics-----\n");
	printf("%d packets transmitted and received, %d packets dropped, total time of %dms\n", transmit, drop, temp);
}

/*
 * Return the entry number  of our IP Address in the arptab
 */
int getEntry(uchar ipAddr[]){
	bool mismatch = FALSE;
	if(existsInTable(ipAddr)){
		int a = 0;
		while(a < ARPENT_LEN){
			//printf("a: %d\n", a); 
			//if it is not in the empty state, compare IP Addresses
			if(arptab.arps[a].state != ARPENT_STATE_EMPTY){
				mismatch = FALSE;
				int u = 0;
				for(u = 0; u < IP_ADDR_LEN; u++){
					//printf("getEntry: %02X : %02X\n",arptab.arps[a].ipAddress[u], ipAddr[u]);
					if(arptab.arps[a].ipAddress[u] != ipAddr[u]){
						mismatch = TRUE;
						break;
					}
				}
					
				//no mismatch was found
				if(!mismatch){
					//printf("getMac returning: ");
					/*int w = 0;
					for(w = 0; w < ETH_ADDR_LEN; w++){
						printf("%02X ", arptab.arps[a].macAddress[w]);
					}
					printf("\n");*/
					return a;

				//mismatch is true, next entry	
				}else{
					//printf("mismatch increment\n");
					a++;
				}
			//its in the free state, next entry
			}else{
				//printf("free state increment\n");
				a++;
			}
		}
	}

	printf("Returning SYSERR in getEntry, no entry found based on IP Address\n");
	return SYSERR;		
}
