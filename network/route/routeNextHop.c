#include <xinu.h>

/*
 * given a destination IP, search the routing table and return
 * index of the gateway in the table
 */
int routeNextHop(uchar destinationIP[]){
	
	int k = 0; 
	for(k = 0; k < ROUTEENT_LEN; k++){
		
		bool mismatch = FALSE;

		int s = 0;
		for(s = 0; s < IP_ADDR_LEN; s++){
			if(destinationIP[s] == routeTab.routes[k].destNetwork[s]){
				printf("match\n");
			}else{
				mismatch = TRUE;
				break;
			}
		}
		if(!mismatch){
			printf("Found destination IP in routing Table, returning entry index\n");
			return k;
		}
	}
	
	//default entry?
	return 9; 
}
