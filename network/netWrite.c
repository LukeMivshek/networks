#include <xinu.h>

void netWrite(uchar[], uchar[], int, ushort);
void printNumberedPacket(uchar[],int);

void netWrite(uchar data[], uchar MAC[],int size, ushort type){

	int len = size + ETHER_SIZE;
	uchar packet[len];
	bzero(packet, len);
	struct ethergram *ethergram = (struct ethergram*)packet;
	
	control(ETH0, ETH_CTRL_GET_MAC, (ulong)ethergram->src, 0);
	memcpy(ethergram->dst, MAC, ETH_ADDR_LEN);
	ethergram->type = htons(type);

	memcpy(ethergram->data, data, size);

	//printNumberedPacket(packet, len);

	write(ETH0, packet, len);

}
