#include <xinu.h>

bool existsInTable(uchar[]);

void gatewayCheck(){
	if(existsInTable(routeTab.routes[0].gateway)){
		printf("Gateway MAC resolved\n");
	}else{
		printf("Gateway MAC failed resolve\n");
	}
	
	
	kill(currpid);
}
