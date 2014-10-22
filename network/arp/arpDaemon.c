#include <xinu.h>

int arpResolve(uchar*, uchar*);
struct packet *arpD_pktPointer;
struct packet arpD_pkt;
void printPacket(uchar[], int);

bool isOurRequest(void);
bool isOurReply(void);
bool hasBeenUpdated;
bool updateArpTable(uchar * mac, uchar * ip);

uchar *ipAdr;
uchar *newMac;

void arpDaemon(){
	printf("Started arpDaemon\n");

	while(1){
		//recieve arp packets from netDaemon
		message msg = receive();
	
		//create a packet from the sent memory address
		arpD_pktPointer = (struct packet*)msg;
		arpD_pkt = *arpD_pktPointer;
		
		//free the memory we used for sending the packet
		free((void *) msg);
		
		uchar tempIp[IP_ADDR_LEN];
		uchar tempMac[ETH_ADDR_LEN];
		
		bzero(tempMac, ETH_ADDR_LEN);		

		ipAdr = tempIp;
		newMac = tempMac;

		//check if its a reply
		if(arpD_pkt.payload[21] == ARP_OPCODE_REPLY){
			//now check if its our reply
			if(isOurReply()){
				printf("Arp packet is our reply\n");
				memcpy(newMac, &arpD_pkt.payload[22], ETH_ADDR_LEN);
				memcpy(ipAdr, &arpD_pkt.payload[28], IP_ADDR_LEN);

				//update the global table with the incoming mac
				updateArpTable(newMac, ipAdr);
				hasBeenUpdated = TRUE;
			}

		}else if(arpD_pkt.payload[21] == ARP_OPCODE_REQUEST){
			if(isOurRequest()){
				printf("Found ARP Request Directed at us\n");
				sendArpReply(arpD_pkt.payload);
			}	
		}else{
			printf("ARP is of unknown type\n");
			printPacket(arpD_pkt.payload, 50);
		}
	}
}

bool isOurReply(){
	
	//set flag to true. if we make it through the for loop without it being set to false, its our ip
	int i = 0;
	for(i = 0; i < IP_ADDR_LEN; i++){
		//printf("comparing arpD packet: %02X with my IP: %02X\n", arpD_pkt.payload[i+38], myIP[i]);
		if(arpD_pkt.payload[i+38] == myIP[i]){
			//printf("match\n");
		}else{
			printf("Mismatch IP Addresses not our reply\n");
			return FALSE;
		}
	}
	
	//printf("incoming packet: \n");
	//printPacket(arpD_pkt.payload, 64);

	printf("isOurReply returning true\n");
	return TRUE;	
}


int arpResolve(uchar *ipaddr, uchar *mac){
	/*
	printf("In arp resolve, ip: ");
	hasBeenUpdated = FALSE;
	int v = 0; 
	for(v = 0; v < IP_ADDR_LEN; v++){
		printf("%02X", ipaddr[v]);
	}
	printf("\n");
	*/

	int tries = 0;

	while(tries < 3){
		printf("Attempt %d in arpResolve\n", tries);

		printf("Sending Arp Resolve packet\n");
		sendArpResolvePacket(ipaddr);
	
		//sleep, then check if we found our arp reply by flag thats set if its found
		sleep(1000);
		//if the table count doesn't equal what it did before sleep
		if(hasBeenUpdated){
			printf("Returning OK in arp Resolve, mac set to %02X\n", mac);
			hasBeenUpdated = FALSE;
			return OK;
		}else{
			//try again
			tries++;
		}
	}

	printf("Returning SYSERR at end of arp resolve after %d tries\n", tries);
	return SYSERR;
}

/*
 * update the arp table with the new mac and ipaddress
 * first, search for an entry with an emptry state and use that
 * if not there, find age 9, update its contents and set its age to 0
 * return SYSERR if nothing is updated
 */
bool updateArpTable(uchar * mac, uchar * ipaddr){
	printf("Updating Table\n");
	
	int o;
	//Update table at arpnum sopt, arpnum being set to the next editable spot
	for(o = 0; o < ARPENT_LEN; o++){
		if(arptab.arps[o].age < arptab.arps[arptab.arpnum].age){
			arptab.arps[o].age = arptab.arps[o].age + 1;
		}
	}
	arptab.arps[arptab.arpnum].age = 0;
	memcpy(&arptab.arps[arptab.arpnum].ipAddress, ipaddr, IP_ADDR_LEN);
	memcpy(&arptab.arps[arptab.arpnum].macAddress, mac, ETH_ADDR_LEN);
	arptab.arps[arptab.arpnum].pid = 0;

	printf("Returning True in updateArpTable, updated entry: %d\n",arptab.arpnum);
	return TRUE;
}

/*
 * check the ip in the incoming arp packet. if its ours the request is directed at us, after this returns true we will send a reply
 */
bool isOurRequest(void){

	int k = 0; 
	for(k = 0; k < IP_ADDR_LEN; k++){
			//printf("comparing arpD packet: %02X with my IP: %02X\n", arpD_pkt.payload[k+38], myIP[k]);
		if(arpD_pkt.payload[k+38] != myIP[k]){
			//printf("Mismatch IP Addresses, not a request for us\n");
			return FALSE;
		}
	}
	printf("isOurRequest returning True\n");
	return TRUE;
}
