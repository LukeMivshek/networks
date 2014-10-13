/**
 * @file netInit.c
 * @provides netInit.
 * 
 * Edited by Luke Mivshek and Casey French
 */
/* Embedded Xinu, Copyright (C) 2008.  All rights reserved. */

#include <xinu.h>

//struct arptable arptab;
int numarp = 0;
void zeroTable(void);

uchar myIP[IP_ADDR_LEN];

/** Initialize network interface.
 */
void netInit(void)
{
	open(ETH0);
	int i;

	//Initializing arptab semaphore and counter (arpnum)
	arptab.arpsem = semcreate(1);	
	arptab.arpnum = 0;

	zeroTable();

	/*This is the only place netDaemonPid should ever be edited or assigned a value*/
	//spawn netdaemon, which spawns icmp, dhcp and arp daemons
	ready(create((void *)netDaemon, INITSTK, 40, "net_daemon", 1, ETH0), 0);
	
	sleep(10);

	return;
}

void zeroTable(void){

	int i = 0;
	//Initializing all entires in the arptab to the empty state 
	for(i = 0; i < ARPENT_LEN; i++){
		arptab.arps[i].state = ARPENT_STATE_EMPTY;
		arptab.arps[i].age = i;
		int v = 0;
		for(v = 0; v < IP_ADDR_LEN; v++){
			arptab.arps[i].ipAddress[v] = 0x00;
		}

	}
}
