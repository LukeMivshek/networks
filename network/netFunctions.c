/* Includes multiple funtions to be used across the network funtions when necessary */

#include <xinu.h>

int parseIP(char*, uchar*);
int parseCIDR_Mask(char*);
int parse(char[]);
void printIP(uchar[]);
void printNumberedPacket(uchar[], int);
void printPacket(uchar[], int);
void printDecimalIP(uchar[]);

void printDecimalIP(uchar ip[]){
	char string[16];

	sprintf(string, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

	string[15] = '\0';
	printf("%-15s", string);
}

int parseIP(char *ipstr, uchar *ip){
	bzero(ip, IP_ADDR_LEN);
	int index = 0;
	char *ip2 = ipstr;

	while(*ipstr && index < IP_ADDR_LEN){
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

/*
 * takes char array classless IP Address with \mask
 * returns mask int
 */
int parseCIDR_Mask(char *str){
	//uchar mask[IP_ADDR_LEN];
	//bzero(mask, IP_ADDR_LEN);

	//uchar* maskPtr;

	int n = 0;
	
	bool isMask = FALSE;
	while(*str){
		if(isMask){
			n *= 10;
			n += *str - '0';
		}else if ((char)*str == '/'){
			isMask = TRUE;
		}
		str++;
	}
	
/*	
	int block = n / 8;
	int count = 0;
	while(count < block){
		mask[count] = 0xFF;
		count++;
	}
	
	int end = n % 8;
	
	if(n == 32){
		mask[3] = 0xFF;
	}else{
		mask[block] = (0xFF << (8-end));
	}

	count = block;
	
	while(count < IP_ADDR_LEN){
		mask[count] = 0x00;
		count++;
	}

	maskPtr = &mask[0];
*/
	
	return n;	
}

int parse(char numStr[]){
	int num = 0;
	char *numptr = numStr;
	while(*numptr){
		if(isdigit((uchar)*numptr)){
			num *= 10;
			num += *numptr - '0';	
		}else{
			printf("Incorrect argument to be parsed, should be a numerical value\n");
			return SYSERR;
		}
		numptr++;
	}
	return num;
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
