/* Includes multiple funtions to be used across the network funtions when necessary */

#include <xinu.h>

int parseIP(char*, uchar*);
void printIP(uchar[]);
void printNumberedPacket(uchar[], int);
void printPacket(uchar[], int);

//uchar parseICMPMAC(uchar []);
//uchar parseICMPIP(uchar []);

int parseIP(char *ipstr, uchar *ip){
	bzero(ip, 4);
	int index = 0;
	char *ip2 = ipstr;

	while(*ipstr){
		if(isdigit((uchar)*ipstr)){
			ip[index] *= 10;
			ip[index] += *ipstr - '0';
		}else{
			index++;
		}
			ipstr++;
	}
	return OK;
}

void printIP(uchar ip[]){
	int d = 0;
	for(d = 0; d < IP_ADDR_LEN; d++){
		printf("%d ", ip[d]);
	}
}

void printNumberedPacket(uchar packet[], int length){
	int m = 0;
	int count = 0;
	printf("0 - 7: ");
	for(m = 0; m < length; m++){
		printf("%02X ", packet[m]);
		if(count == 7){
			printf("\n");
			count = 0;
			printf("%d - %d: ", m+1, m+8);
		}else{
			count++;
		}
	}
	printf("\n");
}

void printPacket(uchar packet[], int length){
        int i = 0;
        int count = 1;
        int count2 = 1;
        while(i < length){
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
