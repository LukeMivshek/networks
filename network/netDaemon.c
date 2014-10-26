/*
 * edited by Luke Mivshek and Casey French
 */
#include <xinu.h>

int Get_Ethergram_Type(uchar[]);

void printPacket(uchar[], int);

bool Is_Our_Offer(int,int,int,int);
bool Is_Our_ACK(int, int, int, int);

bool Is_Our_Echo_Reply(uchar[]);
bool Is_Echo_Request(uchar[]);

void Print_Source_Address(uchar[]);
struct packet netD_pkt;
struct packet *pktPointer;

void setMyIP(uchar[]);

void printNumberedPacket(uchar packet[], int length);

void Print_Netstat(uchar[]);

void Print_Entire_Packet(uchar[]);

bool ipSet = FALSE;

bool snoopActive;

/**
 * reads from ethernet device and decides what to do with packets.
 **/
void netDaemon(int ETH_0){

	//uchar packet[PKTSZ];
	//uchar dhcpPacket[PKTSZ];
	//uchar arpPacket[PKTSZ];

	bzero(netD_pkt.payload,PKTSZ);
	//bzero(dhcpPacket,PKTSZ);
	//bzero(arpPacket,PKTSZ);

	//pids to refernce processes	
	int dhcp_pid;
	int arp_pid;
	int icmp_pid;

	//spawn dhcpClient and get our own IP
	ready(dhcp_pid = create((void *)dhcpClient, INITSTK, 40, "dhcp_client", 1,ETH_0), 0);
	
	snoopActive = FALSE;
	//spawn arpDaemon
	ready(arp_pid = create((void *)arpDaemon, INITSTK, 41, "arp_daemon",0),0);	

	//spawn icmpDaemon
	ready(icmp_pid = create((void *)icmpDaemon,INITSTK, 41, "icmp_daemon", 0), 0);
	//send dhcp discover, method in packet.c
	sendDiscoverPacket();
	
	ipSet = FALSE;

	while(1){
	
		//Read packets in and send there where they belong
		netD_pkt.length = read(ETH_0, netD_pkt.payload, PKTSZ);
		netD_pkt.interface = ETH_0;
		//Get the packet type and handle them appropriately
		int packetType = Get_Ethergram_Type(netD_pkt.payload);
		
		if(snoopActive){
			Print_Entire_Packet(netD_pkt.payload);
		}			


		/* Handling ARP Packets 
 	         * (Homework 3)
 	         * send packet over to arpDaemon to be evaluated 
 	         */
		if(packetType == ETYPE_ARP){	
		
			//malloc space for the packet being sent to arpDaemon to handle
			pktPointer = malloc(sizeof(struct packet));
			
			//Copy current packet to amlloced space
			memcpy(pktPointer, &netD_pkt, sizeof(struct packet));
			
			//send the arp daemon the packet start
			send(arp_pid, (int)pktPointer);

			sleep(200);
		}
		
		//handling IPV4 packets (homework 2)
		if(packetType == ETYPE_IPv4){
			//is ICMP
 			if(netD_pkt.payload[23] == IPv4_PROTO_ICMP){

				if(Is_Our_Echo_Reply(netD_pkt.payload)){
				
					//message pass to the shell, using the pid from the packet
					pktPointer = malloc(sizeof(struct packet));
					
					memcpy(pktPointer, &netD_pkt, sizeof(struct packet));
					int shellPid = netD_pkt.payload[38];
					printf("%d bytes received from %d.%d.%d.%d: icmp_seq = %d ", netD_pkt.payload[17],netD_pkt.payload[26], netD_pkt.payload[27], netD_pkt.payload[28], netD_pkt.payload[29], netD_pkt.payload[40]);
					send(shellPid, 1);
	
				}else if(Is_Echo_Request(netD_pkt.payload)){
	
					pktPointer = malloc(sizeof(struct packet));
			
					//Copy current packet to amlloced space
					memcpy(pktPointer, &netD_pkt, sizeof(struct packet));
					//send to icmp daemon
					send(icmp_pid, (int)pktPointer);
				}else{
					printf("Incoming ICMP is neither request or reply\n");
				}
			}

			//since we know its an ipv4, we know where dhcp stuff is
			int packet_tag = netD_pkt.payload[282];
			int packet_length = netD_pkt.payload[283];
			int packet_value = netD_pkt.payload[284];
			int packet_transactionID;
			
			//get the transaction ID from the packet
			memcpy(&packet_transactionID,&netD_pkt.payload[46],4);
			
			if(ipSet == FALSE && Is_Our_Offer(packet_tag,packet_length,packet_value,packet_transactionID) == TRUE){
				printf("Found Our Offer Packet\n");
				
				//malloc space for the packet being sent to arpDaemon to handle
				pktPointer = malloc(sizeof(struct packet));
			
				//Copy current packet to amlloced space
				memcpy(pktPointer, &netD_pkt, sizeof(struct packet));

				//send the arp daemon the packet start
				send(dhcp_pid, (int)pktPointer);

			}
				
			if(Is_Our_ACK(packet_tag,packet_length, packet_value, packet_transactionID) ==TRUE){
				printf("Found Our ACK Packet\n");
				setMyIP(netD_pkt.payload);
				Print_Netstat(netD_pkt.payload);	
			}
			
		}

		bzero(netD_pkt.payload, PKTSZ);
	}
}

/*
 * This function returns the type of ethergram
 * Return syserror if we dont have a defined type
 */
int Get_Ethergram_Type(uchar packet[]){
	if (packet[12] == 8 && packet[13] == 0){
		//printf("Found IPv4 Ethernet Packet\n");
		return ETYPE_IPv4;
	}else if(packet[12] == 8 && packet[13] == 6){
		//printf("Found ARP Ethernet Packet\n");
		return ETYPE_ARP;
	}else if(packet[12] == 32 && packet[13] == 50){
		//printf("Found FISH Ethernet Packet\n");
		return ETYPE_FISH;
	}else{
		//printf("Found undefined type packet\n");
		return SYSERR;
	}
}


void Print_Netstat(uchar packet[]){
	
	printf("---------------netstat---------------\n");
	printf("Opcode:                   %02X\n", packet[42]);
	printf("Hardware Type:            %02X\n", packet[43]);
	printf("Hardware Address Length:  %02X\n", packet[44]);
	printf("Hop count:                %02X\n", packet[45]);  
	printf("Transaction ID:           %02X\n", (packet[46]+packet[47]+packet[48]+packet[49]));
	printf("Time Elapsed:             %02X%02X\n", packet[50], packet[51]);
	printf("Client IP:                %02X.%02X.%02X.%02X\n", packet[54], packet[55], packet[56], packet[57]);
	printf("Your IP:                  %02X.%02X.%02X.%02X\n", packet[58], packet[59], packet[60], packet[61]);
	printf("Server IP:                %02X.%02X.%02X.%02X\n", packet[62], packet[63], packet[64], packet[65]);
	printf("Router IP:                %02X.%02X.%02X.%02X\n", packet[66], packet[67], packet[68], packet[69]);
	printf("-------------------------------------\n");
    	
}


/*
 * some conditions to check if the offer packet is the one we requested
 * if it makes it to the end, it is our offer and we return true
 */
bool Is_Our_Offer(int tag, int length, int value,int transID){	
	//transation ID we agreed upon
	int transactionID = htonl(0x6b69746e);
	
	if(tag != 0x35){
		//printf("Not our offer because of tag.\n");
		return FALSE;
	}
	if(length != 0x01){
		//printf("Not our offer because of length\n");
		return FALSE;
	}
	if(value != DHCP_MESSAGE_OFFER){
		return FALSE;
	}
	if(transID != transactionID){
		//printf("Not our offer because of transaction ID mismatch\n");
		return FALSE;
	}
	return TRUE;	
}

/*
 * if we make it to the end of this function, we return true because
 * it passed the conditions to be our ACK
 */
bool Is_Our_ACK(int tag, int length, int value, int transID){
	
	//transation ID we agreed upon
	int transactionID = htonl(0x6b69746e);
	
	if(tag != 0x35){
		//printf("Not our offer because of tag.\n");
		return FALSE;
	}

	if(length != 0x01){
		//printf("Not our offer because of length\n");
		return FALSE;
	}

	if(value != DHCP_MESSAGE_ACK){
		return FALSE;
	}

	if(transID != transactionID){
		//printf("Not our transaction ID");
		return FALSE;
	}
	
	ipSet = TRUE;

	return TRUE;
}

/*
 * Takes a packet and prints the source address
 */
void Print_Source_Address(uchar packet[]){
	
	printf("Source IP Address: ");
	int b = 6;
	for(b = 6; b < 12;  b++){
		printf("%d ", packet[b]);
	}		
	printf("\n");
}

void setMyIP(uchar packet[]){
	memcpy(&myIP[0], &packet[58],IP_ADDR_LEN);
	
	printf("My IP set as: ");
	int m = 0; 
	for(m = 0; m < IP_ADDR_LEN; m++){
		if(m == (IP_ADDR_LEN-1)){
			printf("%d\n",myIP[m]);
		}else{
			printf("%d.", myIP[m]);
		}
	}
	
	//set the global flag that we are not expecting any dhcp packets
	ipSet = TRUE;
}

/*
 * given a packet, check that the type is a reply and the ip matches ours,
 * meaning it is an icmp reply directed at us
 */
bool Is_Our_Echo_Reply(uchar packet[]){
	if(packet[34] == ICMP_REPLY){
		int x = 0; 
		for(x = 0; x < IP_ADDR_LEN; x++){
			if(packet[30 + x] != myIP[x]){
				printf("My IP mismatch packet destination address, returning false\n");
				return FALSE;
			}
		}
		return TRUE;
	}else{
		//printf("Returning false in Is_Echo_Reply, not type ICMP_REPLY\n");
		return FALSE;
	}
	printf("Returning SYSERR in Is_Echo_Reply\n");
	return SYSERR;
}

/*
 * given a packet, chech that it is an Echo request directed at us
 */
bool Is_Echo_Request(uchar packet[]){
	return SYSERR;
}
