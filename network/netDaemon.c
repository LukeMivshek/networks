/*
 * edited by Luke Mivshek and Casey French
 */
#include <xinu.h>

int Get_Ethergram_Type(uchar[]);
void printPacket(uchar[], int);
bool Is_Our_Offer(int,int,int,int);
bool Is_Our_ACK(int, int, int, int);
void Print_Source_Address(uchar[]);
struct packet netD_pkt;
struct packet *pktPointer;
void setMyIP(uchar[]);

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
	
	//send dhcp discover, method in packet.c	
	sendDiscoverPacket();

	//spawn arpDaemon
	ready(arp_pid = create((void *)arpDaemon, INITSTK, 41, "arp_daemon",0),0);	

	//spawn icmpDaemon
	ready(icmp_pid = create((void *)icmpDaemon,INITSTK, 41, "icmp_daemon", 0), 0);
	
	while(1){
	
		//Read packets in and send there where they belong
		netD_pkt.length = read(ETH_0, netD_pkt.payload, PKTSZ);
		netD_pkt.interface = ETH_0;
		//Get the packet type and handle them appropriately
		int packetType = Get_Ethergram_Type(netD_pkt.payload);
		
		/* Handling ARP Packets 
 	         * (Homework 3)
 	         * send packet over to arpDaemon to be evaluated 
 	         */
		if(packetType == ETYPE_ARP){

			//Debug print
			//Print_Packet_Fields(netD_pkt.payload, ARPSZ);
			
			//malloc space for the packet being sent to arpDaemon to handle
			pktPointer = malloc(sizeof(struct packet));
			
			//Copy current packet to amlloced space
			memcpy(pktPointer, &netD_pkt, sizeof(struct packet));
			
			//Debug prints
			//printf("Sending packet to arpDaemon\n");
			//printf("Packet Start in ND: %X\n", pktPointer);

			//send the arp daemon the packet start
			send(arp_pid, (int)pktPointer);

			sleep(200);
		}
		
		//handling IPV4 packets (homework 2)
		if(packetType == ETYPE_IPv4){
			
			//since we know its an ipv4, we know where dhcp stuff is
			int packet_tag = netD_pkt.payload[282];
			int packet_length = netD_pkt.payload[283];
			int packet_value = netD_pkt.payload[284];
			int packet_transactionID;
			
			//get the transaction ID from the packet
			memcpy(&packet_transactionID,&netD_pkt.payload[46],4);
			
			//printf("Tag: %02X Length: %02X Value: %02X Transaction ID: %02X\n",packet_tag,packet_length,packet_value,packet_transactionID);
			//print entire packet
			//printPacket(netD_pkt.payload, PKTSZ);
			if(Is_Our_Offer(packet_tag,packet_length,packet_value,packet_transactionID) == TRUE){
				printf("Found Our Offer Packet\n");
				
				setMyIP(netD_pkt.payload);
	
				//print source
				//Print_Source_Address(packet);
				
				//malloc space for the packet being sent to arpDaemon to handle
				pktPointer = malloc(sizeof(struct packet));
			
				//Copy current packet to amlloced space
				memcpy(pktPointer, &netD_pkt, sizeof(struct packet));
				
				//Debug prints
				//printf("Sending packet to arpDaemon\n");
				//printf("Packet Start in ND: %X\n", pktPointer);
				
				//printf("Sending packet pointer to dhcpClient\n");

				//send the arp daemon the packet start
				send(dhcp_pid, (int)pktPointer);

				//sleep to keep natDaemon from working too hard
				sleep(200);

			}
				
			if(Is_Our_ACK(packet_tag,packet_length, packet_value, packet_transactionID) ==TRUE){
				printf("Found Our ACK Packet\n");	
			}
			
		}
	
		/*	
		printf("Netdaemon sleeping...");		
		sleep(2000);
		printf("Awake, ");

		printf("end of packet read.\n\n");
		*/
		bzero(netD_pkt.payload, PKTSZ);

		//prett sure we don't need this. its up top
		//read(ETH_0, netD_pkt.payload, PKTSZ);
	}
}

/*
 * This function returns the type of ethergram
 * Return syserror if we dont have a defined type
 */
int Get_Ethergram_Type(uchar packet[]){
	//printf("12: %02X 13: %02X ", packet[12], packet[13]);
	if (packet[12] == 8 && packet[13] == 0){
		printf("Found IPv4 Ethernet Packet\n");
		return ETYPE_IPv4;
	}else if(packet[12] == 8 && packet[13] == 6){
		printf("Found ARP Ethernet Packet\n");
		return ETYPE_ARP;
	}else if(packet[12] == 32 && packet[13] == 50){
		printf("Found FISH Ethernet Packet\n");
		return ETYPE_FISH;
	}else{
		//printf("Found undefined type packet\n");
		return SYSERR;
	}
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
		//printf("Not our offer because of value is not an offer\n");
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
		printf("Not our offer because of tag.\n");
		return FALSE;
	}

	if(length != 0x01){
		printf("Not our offer because of length\n");
		return FALSE;
	}

	if(value != DHCP_MESSAGE_ACK){
		printf("Not our ACK because value is not an ACK\n");
		return FALSE;
	}

	if(transID != transactionID){
		printf("Not our transaction ID");
		return FALSE;
	}
	
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
		//printf("%02X ", myIP[m]);
	}
}
