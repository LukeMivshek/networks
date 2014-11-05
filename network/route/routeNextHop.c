#include <xinu.h>

/*
 * given a destination IP, search the routing table and return
 * index of the gateway in the table
 */
int routeNextHop(uchar destinationIP[]){
	
	int k = ROUTEENT_LEN; 
	for(k = (ROUTEENT_LEN-1); k >= 0; k--){
		
		bool mismatch = TRUE;
		bool zeros = TRUE;

		int s = 0;
		for(s = 0; s < IP_ADDR_LEN; s++){
		if(routeTab.routes[k].interface != -1){
				if((destinationIP[s]&routeTab.routes[k].netmask[s]) == routeTab.routes[k].destNetwork[s]){
					mismatch = FALSE;
				}else{
					mismatch = TRUE;
					break;
				}
			}
		}
		if(!mismatch){
			//printf("Found destination IP in routing Table, returning entry index: %d\n", k);
			for(s = 0; s < IP_ADDR_LEN; s++){
				if(routeTab.routes[k].gateway[s] != 0){
					zeros = FALSE;
				}
			}
			if(!zeros){
				return k;
			}else{
				return -1;
			}
		}
	
	}
	
	//default entry is 0
	return -2; 
}
