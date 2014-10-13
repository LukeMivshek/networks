#include <xinu.h>

void Print_Full_Packet(uchar[]);

struct ethergram *cli_ethergram;
struct ipgram *cli_ipgram;
struct udpgram *cli_udpgram;
struct dhcpgram *cli_dhcpgram;

int ipcount = 0;

void dhcpClient(int packet2){

	printf("Started dhcpClient\n");

	uchar packet[PKTSZ];
	
	struct ethergram *cli_ethergram = (struct ethergram*)packet;
	struct ipgram *cli_ipgram = (struct ipgram*)cli_ethergram->data;
	struct udpgram *cli_udpgram = (struct udpgram*)cli_ipgram->opts;
	struct dhcpgram *cli_dhcpgram = (struct dhcpgram*)cli_udpgram->data;	

	while(1){	
		bzero(packet, PKTSZ);

		//recieve the message from the netDaemon
		message msg = receive();
	
		//cast to int
		int pckstrt = (int)msg;
		uchar *start = (uchar *)pckstrt;
		
		//used to check the message pass with netdaemon	
		//printf("DHCP Client thinks the address is: %d\n", pckstrt);
		//memcpy(&packet[0],(void *)pckstrt, 1);
		//printf("Packet pointer received in dhcpClient\n");
		//Print_Full_Packet(start);
		//Print_Full_Packet(packet);
		int m = 0;

		//for(m = 0; m < ETH_ADDR_LEN; m++){
		//ethergm->dst[m] = 0xFF;
		//}

		//Dest Address	
		for(m = 0; m < ETH_ADDR_LEN; m++){
			cli_ethergram->dst[m] = 0xFF;
		}
		//Src Address
		control(ETH0, ETH_CTRL_GET_MAC, (ulong)cli_ethergram->src, 0);
		//Type
		cli_ethergram->type = htons(ETYPE_IPv4);

		// Header Length to give 4&5 in single bits for length
		cli_ipgram->ver_ihl = 69;
		cli_ipgram->tos = IPv4_TOS_ROUTINE; 
		cli_ipgram->len = 0; //temp
		//TODO len is end of options to beginning - 14 for the ethergram
		cli_ipgram->id = ipcount;
		//update the id counter for the next packet
		ipcount++;
		//Enter in flags eventually TODO
        	cli_ipgram->flags_froff = 0x0000000000;
		//cli_ipgram->ttl = IPv4_TTL;
		cli_ipgram->proto = IPv4_PROTO_UDP;
		cli_ipgram->chksum = 0; //temp
		//TODO set checksum at end

		int zeros = 0x00000000;
		int fs = 0xFFFFFFFF;
		int zeros_length = 100;
		int long_zeros[zeros_length];
		int i;
		for(i = 0; i < zeros_length; i++){
			long_zeros[i] = 0x0000;
		}

		memcpy(&cli_ipgram->src[0], &zeros, 4);
		memcpy(&cli_ipgram->dst[0], &fs, 4);

		cli_udpgram->srcPort = htons(0x44);
		cli_udpgram->dstPort = htons(0x43);
		cli_udpgram->len = 0; //temp
		
		//TODO len is the same as ipgram - ipgram size
		cli_udpgram->chksum = 0; //temp
		//TODO chksum at end
	
		cli_dhcpgram->opcode = DHCP_OPCODE_REQUEST;
		cli_dhcpgram->htype = 0x01;
		cli_dhcpgram->hlen = 0x06;
		cli_dhcpgram->hops = 0x00;
		//kitn 
		int trans_id= htonl(0x6B69746E);
		memcpy(&cli_dhcpgram->id, &trans_id, 4);
		memcpy(&cli_dhcpgram->elapsed, &zeros, 2);
		memcpy(&cli_dhcpgram->flags, &zeros, 2);
		memcpy(&cli_dhcpgram->client, &zeros, IPv4_ADDR_LEN);
		memcpy(&cli_dhcpgram->server, &zeros, IPv4_ADDR_LEN);
		memcpy(&cli_dhcpgram->router, &zeros, IPv4_ADDR_LEN);

		memcpy(&cli_dhcpgram->servname, &long_zeros[0], DHCP_SERVNAME_LEN);
		memcpy(&cli_dhcpgram->bootfile, &long_zeros[0], DHCP_BOOTFILE_LEN);
		
		int magic_cookie1 = 0x63;
		int magic_cookie2 = 0x82;
		int magic_cookie3 = 0x53;
		int magic_cookie4 = 0x63;
	
		cli_dhcpgram->opts[0] = magic_cookie1;
		cli_dhcpgram->opts[1] = magic_cookie2;
		cli_dhcpgram->opts[2] = magic_cookie3;
		cli_dhcpgram->opts[3] = magic_cookie4;
		cli_dhcpgram->opts[4] = 0x35;
		cli_dhcpgram->opts[5] = 0x01;
		cli_dhcpgram->opts[6] = 0x03;
		cli_dhcpgram->opts[7] = 0x32;
		cli_dhcpgram->opts[8] = 0x06;
		cli_dhcpgram->opts[9] = packet[70];
		cli_dhcpgram->opts[10] = packet[71];
		cli_dhcpgram->opts[11] = packet[72];
		cli_dhcpgram->opts[12] = packet[73];
		cli_dhcpgram->opts[13] = packet[74];
		cli_dhcpgram->opts[14] = packet[75];
		cli_dhcpgram->opts[15] = 0xFF;
		int end = (int)&cli_dhcpgram->opts[16];
	
		control(ETH0, ETH_CTRL_GET_MAC, (ulong)cli_dhcpgram->hwaddr, 0);

		//memcpy(&cli_dhcpgram->hwaddr[0], &myMac, ETH_ADDR_LEN);

		cli_ipgram->len = htons(end - (int)&cli_ethergram->data);
		cli_udpgram->len = htons(end - (int)&cli_ipgram->opts);
		cli_ipgram->chksum = checksum((uchar *)cli_ipgram, (4 * (cli_ipgram->ver_ihl & IPv4_IHL)));

		//Print_Full_Packet(packet);
	
		//write packet
		write(ETH0, packet, PKTSZ);
		printf("Sent dhcp reply packet.\n");
	}
}

void Print_Full_Packet(uchar packet[]){
	int i = 0;
	int count = 1;
	int count2 = 1;
	
	while(i < PKTSZ){
		printf("%02X", packet[i]);
		if(count == 2){
			printf(" ");
			count = 0;
		}
		if(count2 == 16){
			printf("\n");
			count2 = 0;
		}
		i++;
		count++;
		count2++;
	}
	printf("\n");
}
