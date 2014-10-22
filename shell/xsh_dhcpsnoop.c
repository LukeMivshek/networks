   /**
 * @file     xsh_dhcpsnoop.c
 * @provides xsh_dhcpsnoop
 *
 * 09SEP
 * What is HLEN? It was never initialized and probably not working. That needs to be solved to get rid of the warning, we were never calling the function the way it was written (GET IPv4 Data Length).
 *
 *
 */
/* Embedded XINU, Copyright (C) 2009.  All rights reserved. */

#include <xinu.h>
#define IPv4 1
#define UDP_Protocol 17


int  Get_Packet_Type(uchar[]);
void Print_Entire_Packet(uchar[]);
void Print_Ethernet_Addresses(uchar[]);
int  Get_Header_Length(uchar[]);
void Print_Time_To_Live(uchar[]);
void Print_IPv4_Addresses(uchar[]);
int  Get_UDP_Data_Length(uchar[]);
void Print_Opcode(uchar[]);
void Print_Hs(uchar[]);
void Print_Transaction_ID(uchar[]);
void Print_Multi_IP(uchar[]);
void Print_Server_Name(uchar[]);
void Print_Bootfile_Name(uchar[]);
void Print_Option(uchar[]);
int Get_Protocol(uchar[]);
void Print_UDP_Port_Numbers(uchar[]);
int Get_IPv4_Data_Length(uchar[],int);
void Assign_Ethergram_Values(void);

struct ethergram *egram;

/**
 * @param nargs count of arguments in args
 * @param args array of arguments
 * @return OK for success, SYSERR for syntax error
 */
command xsh_dhcpsnoop(int nargs, char *args[])
{

	printf("Inside dhcpsnoop");
	uchar packet[PKTSZ];
	uchar *ppkt = packet;
        int i = 0;

        // Zero out the packet buffer.
        bzero(packet, PKTSZ);

        while(1){
                int m = 0;
		read(ETH0, packet, PKTSZ);
		//Assign_Ethergram_Values();
		//printf("%d", egram->type);
                if (Get_Packet_Type(packet)==IPv4){
			Print_Entire_Packet(packet);
                }else{
                        printf("Non IPv4 packet found.\n");
                }
        }
        return OK;
}

void Print_Entire_Packet(uchar packet[]){
	printf("----------START ETHERNET FRAME--------------------\n");
	Print_Ethernet_Addresses(packet);
	Get_Packet_Type(packet);
	printf("----------START PROTOCOL (IPv4) HEADER------------\n");
	Get_IPv4_Data_Length(packet,Get_Header_Length(packet));
	Print_Time_To_Live(packet);
	Get_Protocol(packet);
	Print_IPv4_Addresses(packet);
	printf("----------START PROTOCOL (UDP) HEADER-------------\n");
	Print_UDP_Port_Numbers(packet);
	Get_UDP_Data_Length(packet);
	printf("----------START DHCP HEADER-----------------------\n");
        Print_Opcode(packet);
	Print_Hs(packet);
	Print_Transaction_ID(packet);
	Print_Multi_IP(packet);
	Print_Server_Name(packet);
	Print_Bootfile_Name(packet);
	Print_Option(packet);
	printf("------------END ETHERNET FRAME--------------------\n\n\n\n");
}

//----------------START ETHERNET FRAME--------------------

/* 
 * Bytes 0-5 are the destination address, bytes 6-11 are the source address. 
 * This is the start of the Ethernet Frame.
 */
void Print_Ethernet_Addresses(uchar packet[]){
        int v = 0;

        for(v = 0; v < 6; v++){
                printf("%02X", packet[v]);
                if(v < 5){
                        printf(":");
                }
        }
	printf(" ");
	for(v = 6; v < 12; v++){
		printf("%02X", packet[v]);
		if(v < 11){
			printf(":");
		}
	}
        printf("\n");
}

/*
 * Bytes 12 and 13 show what the upper layer protocol of the packet is
 */
int Get_Packet_Type(uchar packet[]){
        if (packet[12] == 8 && packet[13] == 0){
		printf("Packet type:                      IPv4");
		return IPv4;
	}
	return -1;
}

//----------------START PROTOCOL (IPv4) HEADER-----------------

/*
 * Byte 14 is the version number of the protocol and defines 1/4 of the value of the 
 * length of the header (HLEN x 4 = Actual_HLEN).
 * The first 4 bits of byte 14 are the version number the second four are the length.
 */
int Get_Header_Length(uchar packet[]){
	int length = ((00001111 & packet[14])*4);
	printf("[IPv4]   Header Length:            %d\n", length);
	return length;
}

//TODO: Byte 15 defines service type, print?

/*
 * Bytes 16 & 17 are the total length. Data length is the header length subtracted 
 * from the total length. (DATALEN = TOTLEN - Actual_HLEN)
 */
int Get_IPv4_Data_Length(uchar packet[], int HLEN){
	int length = ((packet[16]+packet[17])-HLEN);
        printf("[IPv4]   Data Length:              %d\n", length);
	return length;
}

/*
 * Bytes 18 & 19 are the Identification, 3 bits of 20 is are the flags, and the rest
 * of 20 as well as 21 are the Fragmentation offset. 22 is the time to live. 
 */
void Print_Time_To_Live(uchar packet[]){
	printf("[IPv4]   Time to Live:             %d\n", packet[22]);
}

/*
 * Byte 23 is the protocol of the next section of the packet.
 */
int Get_Protocol(uchar packet[]){
	int protocol = packet[23];
	if (protocol == UDP_Protocol){
		printf("[IPv4]   Protocol value is %d: UDP\n", UDP_Protocol);
		return UDP_Protocol;
	}
	printf("Returning -1 in Get_Protocol, something proably went wrong");
	return -1;
}

/*
 * Bytes 24 & 25 are the header checksum. Bytes 26-29 and 30-33 are the IPv4 addresses
 */
 void Print_IPv4_Addresses(uchar packet[]){       
	int v = 26;

        for(v = 26; v < 30; v++){
                printf("%02X", packet[v]);
                if(v < 29){
                        printf(":");
                }
        }
	printf(" ");
	for(v = 30; v < 34; v++){
		printf("%02X", packet[v]);
		if(v < 33){
			printf(":");
		}
	}
        printf("\n");
}

//----------------START PROTOCOL (UDP) HEADER-----------------

/*
 * Bytes 34-35 & 36-37 are the source and destination port numbers
 */
void Print_UDP_Port_Numbers(uchar packet[]){
	printf("[UDP]    Source port number:       %d\n", (packet[34]+packet[35]));
	printf("[UDP]    Destination port number:  %d\n", (packet[36]+packet[37]));
}

/*
 * Bytes 38-39 & 40-41 are the total length and checksum
 */
int Get_UDP_Data_Length(uchar packet[]){
	int length = (packet[38]+packet[39]);
	printf("[UDP]    Total Length:             %d\n", length);
	return(length);
}

//----------------START DHCP HEADER-------------------------

/*
 * Byte 42 is the opcode
 */
void Print_Opcode(uchar packet[]){
	printf("[DHCP]   Opcode:                   %02X\n", packet[42]);
}

/*
 * Bytes 43, 44, and 45 are the hardware type, hardware address length, and hop count; respectively
 */
void Print_Hs(uchar packet[]){
	printf("[DHCP]   Hardware Type:            %02X\n", packet[43]);
	printf("[DHCP]   Hardware Address Length:  %02X\n", packet[44]);
	printf("[DHCP]   Hop count:                %02X\n", packet[45]); 
}

/*
 * Bytes 46-49 are the transaction ID
 */
void Print_Transaction_ID(uchar packet[]){
	printf("[DHCP]   Transaction ID:           %02X\n", (packet[46]+packet[47]+packet[48]+packet[49]));
}

/*
 *Bytes 50 & 51 are the time elapsed
 */
void Print_Time_Elapsed(uchar packet[]){
	printf("[DCHP]   Time Elapsed:             %02X\n", (packet[50]+packet[51]));
}

/*
 * Bytes 52 & 53 are flags, bytes 54-57 are the client IP address, bytes 58-61 are your IP
 * Bytes 52 & 53 are flags, bytes 54-57 are the client IP address, bytes 58-61 are your IP
 * address, bytes 62-65 are the servers IP address, and bytes 66-69 are the gateways IP
 * Bytes 52 & 53 are flags, bytes 54-57 are the client IP address, bytes 58-61 are your IP
 * address, bytes 62-65 are the servers IP address, and bytes 66-69 are the gateways IP
 * address.
 */
void Print_Multi_IP(uchar packet[]){
	printf("[DHCP]   Client IP:                %02X.%02X.%02X.%02X\n", packet[54], packet[55], packet[56], packet[57]);
	printf("[DHCP]   Your IP:                  %02X.%02X.%02X.%02X\n", packet[58], packet[59], packet[60], packet[61]);
	printf("[DHCP]   Server IP:                %02X.%02X.%02X.%02X\n", packet[62], packet[63], packet[64], packet[65]);
	printf("[DHCP]   Gateway IP:               %02X.%02X.%02X.%02X\n", packet[66], packet[67], packet[68], packet[69]);
}

/*
 * Bytes 70-85 (the next 16 bytes) are the client hardware address.
 * Bytes 86-149 are the server name
 */
void Print_Server_Name(uchar packet[]){
	int i = 0;
	printf("Server name: \"");
	for (i = 86; i < 150; i++){
		if(packet[i] != 0){
			printf("%02X", packet[i]);
		}
	}
	printf("\"\n");
}

/*
 * Bytes 150-277 are the Bootfile Name.
 */
void Print_Bootfile_Name(uchar packet[]){
	int i = 0;
	printf("Bootfile name: \"");
	for (i = 151; i < 278; i++){
		if(packet[i] != 0){
			printf("%02X ", packet[i]);
		}
	}
	printf("\"\n");
}

/*
 * Bytes 278 + are an option field that can hold options if started with a magic cookie (99.130.83.99)
 */
void Print_Option(uchar packet[]){
	if(packet[278]==0x63 && packet[279]==0x82 && packet[280]==0x53 && packet[281]==0x63){
		printf("Magic Cookie Exists, value 99.130.83.99 matches\n");
		printf("Option %02X, length %02X: REQUEST\n", packet[282], packet[283]);
		
	}else{
		printf("Magic Cookie does not exist.\n");
	}
}
