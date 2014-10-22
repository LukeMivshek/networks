#include <xinu.h>

void printPacket(uchar[], int);
void printIP(uchar[]);
void Build_Ethernet_Frame(void);
void Build_IPv4_Header(void);
void Build_UDP_Header(void);
void Build_DHCP_Header(void);
void sendDiscoverPacket(void);
void sendArpReply(uchar[]);
void sendEchoRequestPacket(uchar[], uchar[], int, int);
void sendEchoReplyPacket(uchar[]);

/* DHCP */ 
struct ethergram *disc_ethergram;
struct ipgram *disc_ipgram;
struct udpgram *disc_udpgram;
struct dhcpgram *disc_dhcpgram;

/* ARP */
struct ethergram *res_ethergram;
struct arpgram *res_arpgram;

/* ICMP */
struct ethergram *ping_ethergram;
struct ipgram *ping_ipgram;
struct icmpgram *ping_icmpgram;

int ipcounter = 0;

void sendDiscoverPacket(){
	
	//declare and zero out packet
	uchar packet[PKTSZ];
	bzero(packet,PKTSZ);
	
	//Create struct pointers for organization
	struct ethergram *disc_ethergram = (struct ethergram*)packet;
	struct ipgram *disc_ipgram = (struct ipgram*)disc_ethergram->data;
	struct udpgram *disc_udpgram = (struct udpgram*)disc_ipgram->opts;
	struct dhcpgram *disc_dhcpgram = (struct dhcpgram*)disc_udpgram->data;	

	//Dest Address
	int m = 0;
	for(m = 0; m < ETH_ADDR_LEN; m++){
		disc_ethergram->dst[m] = 0xFF;
	}

	//Src Address
	//memcpy(&disc_ethergram->src[0], &, ETH_ADDR_LEN);
	control(ETH0, ETH_CTRL_GET_MAC, (ulong)disc_ethergram->src, 0);
	
	//Type
	disc_ethergram->type = htons(ETYPE_IPv4);

	// Header Length to give 4&5 in single bits for length
	disc_ipgram->ver_ihl = 69;
	disc_ipgram->tos = IPv4_TOS_ROUTINE; 
	disc_ipgram->len = 0; //temp
	//TODO len is end of options to beginning - 14 for the ethergram
	disc_ipgram->id = ipcounter;
	//update the id counter for the next packet
	ipcounter++;
	//Enter in flags eventually TODO
        disc_ipgram->flags_froff = 0x0000000000;
	disc_ipgram->ttl = IPv4_TTL;
	disc_ipgram->proto = IPv4_PROTO_UDP;
	disc_ipgram->chksum = 0; //temp
	//TODO set checksum at end

	int zeros = 0x00000000;
	int fs = 0xFFFFFFFF;
	int zeros_length = 100;
	int long_zeros[zeros_length];
	int i;

	for(i = 0; i < zeros_length; i++){
		long_zeros[i] = 0x0000;
	}

	memcpy(&disc_ipgram->src[0], &zeros, 4);
	memcpy(&disc_ipgram->dst[0], &fs, 4);

	disc_udpgram->srcPort = htons(0x44);
	disc_udpgram->dstPort = htons(0x43);
	disc_udpgram->len = 0; //temp
	//TODO len is the same as ipgram - ipgram size
	disc_udpgram->chksum = 0; //temp
	//TODO chksum at end
	
	disc_dhcpgram->opcode = DHCP_OPCODE_REQUEST;
	disc_dhcpgram->htype = 0x01;
	disc_dhcpgram->hlen = 0x06;
	disc_dhcpgram->hops = 0x00;
	//kitn 
	int trans_id= htonl(0x6B69746E);
	memcpy(&disc_dhcpgram->id, &trans_id, 4);
	memcpy(&disc_dhcpgram->elapsed, &zeros, 2);
	memcpy(&disc_dhcpgram->flags, &zeros, 2);
	memcpy(&disc_dhcpgram->client, &zeros, IPv4_ADDR_LEN);
	memcpy(&disc_dhcpgram->server, &zeros, IPv4_ADDR_LEN);
	memcpy(&disc_dhcpgram->router, &zeros, IPv4_ADDR_LEN);

	control(ETH0, ETH_CTRL_GET_MAC, (ulong)disc_dhcpgram->hwaddr, 0);
	
	memcpy(&disc_dhcpgram->servname, &long_zeros[0], DHCP_SERVNAME_LEN);
	memcpy(&disc_dhcpgram->bootfile, &long_zeros[0], DHCP_BOOTFILE_LEN);
	int magic_cookie1 = 0x63;
	int magic_cookie2 = 0x82;
	int magic_cookie3 = 0x53;
	int magic_cookie4 = 0x63;
	
	disc_dhcpgram->opts[0] = magic_cookie1;
	disc_dhcpgram->opts[1] = magic_cookie2;
	disc_dhcpgram->opts[2] = magic_cookie3;
	disc_dhcpgram->opts[3] = magic_cookie4;
	disc_dhcpgram->opts[4] = 0x35;
	disc_dhcpgram->opts[5] = 0x01;
	disc_dhcpgram->opts[6] = 0x01;
	disc_dhcpgram->opts[7] = 0x37;
	disc_dhcpgram->opts[8] = 0x08;
	disc_dhcpgram->opts[9] = 0x01;
	disc_dhcpgram->opts[10] = 0x0F;
	disc_dhcpgram->opts[11] = 0x03;
	disc_dhcpgram->opts[12] = 0x06;
	disc_dhcpgram->opts[13] = 0x10;
	disc_dhcpgram->opts[14] = 0x11;
	disc_dhcpgram->opts[15] = 0x82;
	disc_dhcpgram->opts[16] = 0x93;
	disc_dhcpgram->opts[17] = 0xFF;
	int end = (int)&disc_dhcpgram->opts[18];
	disc_ipgram->len = htons(end - (int)&disc_ethergram->data);
	disc_udpgram->len = htons(end - (int)&disc_ipgram->opts);
	disc_ipgram->chksum = checksum((uchar *)disc_ipgram, (4 * (disc_ipgram->ver_ihl & IPv4_IHL)));

	
	printf("Writing discover packet\n");

	//printPacket(packet, PKTSZ);

	write(ETH0, packet, PKTSZ);
	return;
}

void sendArpResolvePacket(uchar* ipAddress){
	
	//declare and zero out packet
	uchar packet[PKTSZ];
	bzero(packet,PKTSZ);

	//Create struct pointers for organization
	struct ethergram *res_ethergram = (struct ethergram*)packet;
	struct arpgram *res_arpgram = (struct arpgram*)res_ethergram->data;

	int m = 0;
	for(m = 0; m < ETH_ADDR_LEN; m++){
		res_ethergram->dst[m] = 0xFF;
	}

	control(ETH0, ETH_CTRL_GET_MAC, (ulong)res_ethergram->src, 0);

	res_ethergram->type = htons(ETYPE_ARP);
	
	int zeros = 0x00000000;
	
	res_arpgram->htype = ARP_LAN_HTYPE;

	res_arpgram->proto = ARP_NETLAY_PROTO;
	
	res_arpgram->hlen = ARP_HLEN;
	res_arpgram->plen = ARP_PLEN;
	res_arpgram->op = ARP_PKT_OPCODE_REQUEST;
	
	control(ETH0, ETH_CTRL_GET_MAC, (ulong)res_arpgram->srcMacAddress, 0);
	
	memcpy(&res_arpgram->srcIpAddress[0], &myIP[0], IP_ADDR_LEN);
	memcpy(&res_arpgram->dstMacAddress[0], &zeros, ETH_ADDR_LEN);
	memcpy(&res_arpgram->dstIpAddress[0], ipAddress, IP_ADDR_LEN);

	printf("Writing Arp resolve packet to ETH0\n");

	//printArpPacket(packet);
	write(ETH0, packet, PKTSZ);

	printf("End of Arp resolve write\n");
	return;
}

/*
 * build and send an Arp reply to respond to an arp resolve (directed at an IP/mac)
 */
void sendArpReply(uchar packet[]){
	
	uchar replyPacket[PKTSZ];
	bzero(replyPacket,PKTSZ);
	
	//Create struct pointers for organization
	struct ethergram *res_ethergram = (struct ethergram*)replyPacket;
	struct arpgram *res_arpgram = (struct arpgram*)res_ethergram->data;
	
	int m = 0;
	int y = 6;
	for(m = 0; m < ETH_ADDR_LEN; m++){
		res_ethergram->dst[m] = packet[y++];
	}
	
	control(ETH0, ETH_CTRL_GET_MAC, (ulong)res_ethergram->src, 0);

	res_ethergram->type = htons(ETYPE_ARP);
	
	int zeros = 0x00000000;

	res_arpgram->htype = ARP_LAN_HTYPE;
	
	res_arpgram->proto = ARP_NETLAY_PROTO;
	
	res_arpgram->hlen = ARP_HLEN;
	res_arpgram->plen = ARP_PLEN;
	
	res_arpgram->op = ARP_PKT_OPCODE_REPLY;

	control(ETH0, ETH_CTRL_GET_MAC, (ulong)res_arpgram->srcMacAddress, 0);
	
	memcpy(&res_arpgram->srcIpAddress[0], &myIP[0], IP_ADDR_LEN);
	memcpy(&res_arpgram->dstMacAddress[0], &packet[22], ETH_ADDR_LEN);
	memcpy(&res_arpgram->dstIpAddress[0], &packet[28], 4);

	printf("Received packet\n");

	//printArpPacket(packet);

	printf("Writing Arp reply packet to ETH0\n");

	//printArpPacket(replyPacket);

	write(ETH0, replyPacket, PKTSZ);	
	
	printf("End of reply write\n");
	return;
}



/*
 * Send an echo request given an IP Address and Mac Address
 * this is called from sendPing shell command
 */
void sendEchoRequestPacket(uchar MAC[], uchar IP[], int pid, int pingsRemaining){
	printf("Sending echo request\n");

	//declare and zero out packet
	uchar packet[PKTSZ];
	bzero(packet,PKTSZ);

	//Create struct pointers for organization
	struct ethergram *ping_ethergram = (struct ethergram*)packet;
	struct ipgram *ping_ipgram = (struct ipgram*)ping_ethergram->data;
	struct icmpgram *ping_icmpgram = (struct icmpgram*)ping_ipgram->opts;
	
	int y;
	//Dest address
	for(y = 0; y < ETH_ADDR_LEN; y++){
		ping_ethergram->dst[y] = MAC[y];
		//ping_ethergram->src[y] = MAC[y];
	}
	//Src address
	control(ETH0, ETH_CTRL_GET_MAC, (ulong)ping_ethergram->src, 0);
	//Type
	ping_ethergram->type = htons(ETYPE_IPv4);

	//IPv4 header
	ping_ipgram->ver_ihl = 69;
	ping_ipgram->tos = IPv4_TOS_ROUTINE;
	ping_ipgram->len = 0; //temporary
	ping_ipgram->id = 0xED14;
	ping_ipgram->flags_froff = htons(IPv4_FLAG_DF<<13);
	ping_ipgram->ttl = IPv4_TTL;
	ping_ipgram->proto = IPv4_PROTO_ICMP;
	ping_ipgram->chksum = 0; //temporary
	memcpy(&ping_ipgram->src[0], &myIP[0], IP_ADDR_LEN);
	memcpy(&ping_ipgram->dst[0], &IP[0], IP_ADDR_LEN);

	//ICMP header
	ping_icmpgram->type = ICMP_REQUEST;
	ping_icmpgram->code = ICMP_QUERY_CODE;
	ping_icmpgram->chksum = 0; //temporary
	ping_icmpgram->ident = pid;
	ping_icmpgram->seq = pingsRemaining;
	int u;
	for(u = 0; u < 18; u++){
		ping_icmpgram->data[u] = (u + 10);
	}	

	//setting lengths and checksums
	int end = (int)&ping_icmpgram->data[60];
	ping_ipgram->len = htons(end - (int)&ping_ethergram->data);
	//ping_ipgram->len = htons(0x0058);
	ping_icmpgram->chksum = checksum((uchar *)ping_icmpgram, (4 * (end - (int)&ping_ipgram->opts)));
	ping_ipgram->chksum = checksum((uchar *)ping_ipgram, (4 * (ping_ipgram->ver_ihl & IPv4_IHL)));


	printf("Length is %d   ", ping_ipgram->len);
	printf("Sending ping %d to ", pingsRemaining);
	printIP(IP);
	printf("\n");
	printPacket(packet, ARPSZ);
	write(ETH0, packet, 64);
	return;
}


/*
 * Send an echo reply given an IP Address and Mac Address
 * this is called from the icmp daemon
 */
void sendEchoReplyPacket(uchar packet[]){
	printf("Sending echo request\n");
		
	printPacket(packet, ARPSZ);
	
	//Create struct pointers for organization
	struct ethergram *ping_ethergram = (struct ethergram*)packet;
	struct ipgram *ping_ipgram = (struct ipgram*)ping_ethergram->data;
	struct icmpgram *ping_icmpgram = (struct icmpgram*)ping_ipgram->opts;
	
	int y;
	//Dest address
	for(y = 0; y  < ETH_ADDR_LEN; y++){
		ping_ethergram->dst[y] = ping_ethergram->dst[y];
	}
	//Src address
	control(ETH0, ETH_CTRL_GET_MAC, (ulong)ping_ethergram->src, 0);
	
	//IPv4 header
	ping_ipgram->chksum = 0; //temporary
	memcpy(&ping_ipgram->dst[0], &ping_ipgram->src[0], IP_ADDR_LEN);
	memcpy(&ping_ipgram->src[0], &myIP[0], IP_ADDR_LEN);

	//ICMP header
	ping_icmpgram->type = ICMP_REPLY;
	ping_icmpgram->code = ICMP_QUERY_CODE;
	ping_icmpgram->chksum = 0; //temporary
	
	//setting lengths and checksums
	int end = (int)&ping_icmpgram->data[60];
	ping_ipgram->len = htons(end - (int)&ping_ethergram->data);
	ping_icmpgram->chksum = checksum((uchar *)ping_icmpgram, (4 * (end - (int)&ping_ipgram->opts)));
	ping_ipgram->chksum = checksum((uchar *)ping_ipgram, (4 * (ping_ipgram->ver_ihl & IPv4_IHL)));

	printf("Writing echo response to ETH0\n");
	printf("Length is %d", ping_ipgram->len);
	printf("Sending ping %d to ", ping_icmpgram->seq);
	printIP(ping_ipgram->dst);
	printf("\n");
	printPacket(packet, ARPSZ);
	write(ETH0, packet, (end-(int)&ping_ethergram));
	return;
}


