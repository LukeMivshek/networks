#include <xinu.h>

#define FRAG_MAX (ETH_MTU - IPv4_HDR_LEN)

void ipWrite(void *, int, uchar *, uchar, uchar[]);
void printNumberedPacket(uchar[], int);	

void ipWrite(void * data, int len, uchar *destIP, uchar protocol, uchar MAC[]){

	static ushort ip_id = 0;

	int sizeOfPacket = len + IPv4_HDR_LEN;
	ushort offset = 0;	
	
	uchar packet[PKTSZ];
	bzero(packet, PKTSZ);

	struct ipgram *ipgram = (struct ipgram*)packet;
	ipgram->ver_ihl = 0x45;
	ipgram->tos = IPv4_TOS_ROUTINE;
	ipgram->proto = protocol;
	ipgram->id = ip_id++;
	ipgram->ttl = IPv4_TTL;
	ipgram->flags_froff = 0;
	memcpy(ipgram->src, myIP, IP_ADDR_LEN);
	memcpy(ipgram->dst, destIP, IP_ADDR_LEN);

	//if the size of the packet plus the size of the ip header is greater than what
	//we can send in one packet, fragment packet
	while(len > 0){
		printf("Fragmenting Packet\n");
		int lenPacket = (len > FRAG_MAX) ? FRAG_MAX : len;
		lenPacket += IPv4_HDR_LEN;
		if (len < FRAG_MAX) {
			if (ipgram->flags_froff != 0) {
				ipgram->flags_froff = IPv4_FLAG_LF;
			} else {
				ipgram->flags_froff = IPv4_FLAG_DF;
			}
		} else {
			ipgram->flags_froff = IPv4_FLAG_MF;
		}
	
		ipgram->flags_froff |= (offset & IPv4_FROFF);
		ipgram->flags_froff = htons(ipgram->flags_froff);
		memcpy(ipgram->opts, data + (offset*8), lenPacket);
		
		//ipgram->len = htons((ushort)(sizeOfPacket));
		ipgram->len = htons((ushort)lenPacket);
	
		ipgram->chksum = 0; //temporary
	
		ipgram->chksum = checksum((uchar *)ipgram, (4 * (ipgram->ver_ihl & IPv4_IHL)));
		printf("lenPacket: %d\n", lenPacket);	
		netWrite(packet, MAC, lenPacket, ETYPE_IPv4);
		len -= FRAG_MAX;
		offset += FRAG_MAX / 8;
	}
}
