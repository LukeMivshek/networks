/**
 * @file network.h
 *
 */
/* Embedded Xinu, Copyright (C) 2008.  All rights reserved. */
#ifndef _NET_H_
#define _NET_H_

#include <kernel.h>

/* Little Endian converters */
# define htons(x) ((((x)>>8) &0xff) | (((x) & 0xff)<<8))
# define ntohs(x) htons(x)
# define htonl(x) ((((x)& 0xff)<<24) | (((x)>>24) & 0xff) | \
		                   (((x) & 0xff0000)>>8) | (((x) & 0xff00)<<8))
# define ntohl(x) htonl(x)


/* Networking Constants */
#define ETH_ADDR_LEN 6          /* MAC address length       */
#define IP_ADDR_LEN  4          /* IP address length        */
#define PKTSZ        ETH_HEADER_LEN + ETH_MTU   /* Maximum packet size      */
#define ARPSZ 64 /* Size of packet we can get away with printing for debugging ARP packets*/

#define ETYPE_IPv4 0x0800
#define ETYPE_ARP  0x0806
#define ETYPE_FISH 0x3250

/* Ethergram header size    */
#define ETHER_SIZE   (ETH_ADDR_LEN * 2 + 2)

/* Minimum payload size */
#define ETHER_MINPAYLOAD 46

/*
 * Ethernet HEADER
 *
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Destination MAC Address                                       
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   ...                           | Source MAC Address             
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   ...                                                           |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Frame Payload Type            |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
struct ethergram             /**< Ethernet Packet Variables             */
{
    uchar dst[ETH_ADDR_LEN];    /**< Ethernet destination MAC Address   */
    uchar src[ETH_ADDR_LEN];    /**< Ethernet source MAC Address        */
    ushort type;             /**< Ethernet Packet Type                  */
    char data[1];            /**< Dummy field that refers to payload    */
};

/* Maximum length of an IPv4 address in dot-decimal notation */
#define IPv4_DOTDEC_MAXLEN    15

/* Masks to point to section of packet */
#define IPv4_IHL			0x0F
#define IPv4_VER			0xF0
#define IPv4_FLAGS			0xE000
#define IPv4_FROFF			0x1FFF
#define IPv4_ADDR_LEN       		4
#define IPv4_TTL 			64

/* IP packet definitions */
#define IPv4_HDR_LEN     	20      /* (Assumes no options or padding) */
#define IPv4_MAX_OPTLEN		40
#define IPv4_MAX_HDRLEN 	IPv4_HDR_LEN + IPv4_MAX_OPTLEN
#define IPv4_MIN_IHL		5
#define IPv4_MAX_IHL		15
#define IPv4_VERSION		4

/* IP Protocols */
#define IPv4_PROTO_ICMP  1
#define IPv4_PROTO_IGMP  2
#define IPv4_PROTO_TCP	  6
#define IPv4_PROTO_UDP   17

/* Flags */
#define IPv4_FLAG_LF		0x0000
#define IPv4_FLAG_MF 		0x2000
#define IPv4_FLAG_DF 		0x4000

/* Types of service */
#define IPv4_TOS_NETCNTRL	0x7
#define IPv4_TOS_INTCNTRL	0x6
#define IPv4_TOS_CRITIC	0x5
#define IPv4_TOS_FLASHOV	0x4
#define IPv4_TOS_FLASH	0x3
#define IPv4_TOS_IM		0x2
#define IPv4_TOS_PRIO		0x1
#define IPv4_TOS_ROUTINE	0x0

/*
 * IPv4 HEADER
 *
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Link-Level Header                                             |
 * | ...                                                           |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |Version| IHL   |Type of Service| Total Length (IHL + Data)     |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |         Identification        |Flags|    Fragment Offset      |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Time to Live  |     Protocol  |       Header Checksum         |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                         Source Address                        |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                       Destination Address                     |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |              Options  & Padding (Variable octets)             |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Data (Variable octets)                                        |
 * | ...                                                           | 
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */

struct ipgram                /**< IPv4 Packet Variables                 */
{
    uchar  ver_ihl;          /**< IPv4 Version and Internet Header Len  */
    uchar  tos;              /**< IPv4 Type of Service                  */
    ushort len;              /**< IPv4 packet length including IHL      */
    ushort id;               /**< IPv4 identification                   */
    ushort flags_froff;      /**< IPv4 Flags and Fragment offset        */
    uchar  ttl;              /**< IPv4 time to live                     */
    uchar  proto;            /**< IPv4 protocol                         */
    ushort chksum;           /**< IPv4 checksum                         */
    uchar  src[IPv4_ADDR_LEN];  /**< IPv4 source                        */
    uchar  dst[IPv4_ADDR_LEN];  /**< IPv4 destination                   */
    uchar  opts[1];          /**< Options and padding is variable       */
};

/* ICMP Query Types and code */
#define ICMP_REQUEST		0x08
#define ICMP_REPLY		0x00
#define ICMP_TIMESTMP_REQUEST	0x13
#define ICMP_TIMESTMP_REPLY	0x14
#define ICMP_QUERY_CODE		0x00

/*
 * ICMP QUERY HEADER
 *
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Type          | Code          | Checksum                      |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Identifier                    | Sequence Number               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Data (Must be at least 60)                                    |
 * | ...                                                           |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */

struct icmpgram			/**< ICMP Packet Variables	    */
{
   uchar type;			/**< ICMP packet type		    */
   uchar code;			/**< ICMP code, reason for msg type */
   ushort chksum;		/**< ICMP checksum                  */
   ushort ident;		/**< ICMP identifier		    */
   ushort seq;			/**< ICMP Sequence number	    */
   uchar data[1];		/**< ICMP data			    */
};

/*
 * UDP HEADER
 *
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Source Port                   | Destination Port              |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Message Length                | Checksum                      |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Data (Variable octets)                                        |
 * | ...                                                           |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */

struct udpgram                  /**< UDP Packet Variables           */
{
    ushort srcPort;             /**< UDP Source port                */
    ushort dstPort;             /**< UDP Destination port           */
    ushort len;                 /**< UDP length, includes header    */
    ushort chksum;              /**< UDP Checksum                   */
    uchar data[1];              /**< UDP data                       */
};

/*
 * DHCP HEADER
 *
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Operation     | Hardware Type | Hardware Len  | Hop Count     |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Transaction ID                                                |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Time elapsed                  | Flags                         |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Client IP (0.0.0.0 if client does not know)                   |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Your IP (client IP sent by server)                            |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Server IP (255.255.255.255 if client does not know)           |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Gateway IP                                                    |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Client Hardware Address                                       |
 * . ...                                                           .
 * | [16 octets]                                                   |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Server name                                                   |
 * . ...                                                           .
 * | [64 octets]                                                   |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Boot file name                                                |
 * . ...                                                           .
 * | [128 octets]                                                  |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Options                                                       |
 * . ...                                                           .
 * | [64 octets]                                                   |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */

#define DHCP_HWFIELD_LEN   16
#define DHCP_SERVNAME_LEN  64
#define DHCP_BOOTFILE_LEN  128

struct dhcpgram                 /**< DHCP Packet Variables          */
{
        uchar  opcode;              /**< DHCP Operation                 */
	uchar  htype;               /**< DHCP hardware type             */
	uchar  hlen;                /**< DHCP hardware address length   */
	uchar  hops;                /**< DHCP hop count allowed         */
	ulong  id;                  /**< DHCP transaction ID            */
	ushort elapsed;             /**< DHCP time elapsed              */
	ushort flags;               /**< DHCP flags                     */
	uchar  client[IPv4_ADDR_LEN];  /**< DHCP client IP address      */
	uchar  yourIP[IPv4_ADDR_LEN];  /**< DHCP your IP address        */
	uchar  server[IPv4_ADDR_LEN];  /**< DHCP server IP address      */
	uchar  router[IPv4_ADDR_LEN];  /**< DHCP gateway IP address     */
	uchar  hwaddr[DHCP_HWFIELD_LEN];  /**< Client hardware address  */
	uchar  servname[DHCP_SERVNAME_LEN];  /**< Server name           */
	uchar  bootfile[DHCP_BOOTFILE_LEN];  /**< Bootfile name         */
    uchar  opts[1];            /**< DHCP Options field              */
};

#define DHCP_OPCODE_REQUEST 1
#define DHCP_OPCODE_REPLY   2

#define DHCP_MAGIC_COOKIE 0x63825363

/* DHCP options,  RFC2132 */
#define DHCP_OPTIONS_SUBNET_MASK    1
#define DHCP_OPTIONS_ROUTER         3
#define DHCP_OPTIONS_DNS_SERVER     6
#define DHCP_OPTIONS_DOMAIN_NAME   15
#define DHCP_OPTIONS_REQUESTED_IP  50
#define DHCP_OPTIONS_LEASETIME     51
#define DHCP_OPTIONS_MESSAGE       53
#define DHCP_OPTIONS_SERVER_ID     54
#define DHCP_OPTIONS_CLIENT_ID     61
#define DHCP_OPTIONS_END           255

/* DHCP Message Types */
#define DHCP_MESSAGE_DISCOVER 0x01
#define DHCP_MESSAGE_OFFER    0x02
#define DHCP_MESSAGE_REQUEST  0x03
#define DHCP_MESSAGE_DECLINE  0x04
#define DHCP_MESSAGE_ACK      0x05
#define DHCP_MESSAGE_NACK     0x06
#define DHCP_MESSAGE_RELEASE  0x07
#define DHCP_MESSAGE_INFORM   0x08

/*
 * ARP HEADER
 *
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Hardware Type                 | Protocal Type                 |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Hardware Len  | Protocol Len  | Operation (Request 1, Reply 2)|
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Source Hardware Address (MAC)                                 |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Source Protocol Address (IP)                                  |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Destination Hardware Address (MAC, empty in request)          |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Destination Protocol Address (IP)                             |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Padding to reach min packet size (I think)                    |
 * . ...                                                           .
 * | ...                                                           |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */


struct arpgram					/**< ARP Packet Variables           */
{
	ushort htype;				/** ARP hardware type               */
	ushort proto;				/** ARP protocol                    */
	uchar  hlen;				/** ARP hardware length             */
	uchar  plen;				/** ARP protocol length             */
	ushort op;				/** ARP operation                   */
	uchar  srcMacAddress[ETH_ADDR_LEN];	/** ARP Source MAC Address      */
	uchar  srcIpAddress[IP_ADDR_LEN];	/** ARP Source IP Address       */
	uchar  dstMacAddress[ETH_ADDR_LEN];	/** ARP Destination MAC Address */
	uchar  dstIpAddress[IP_ADDR_LEN];	/** ARP Destination IP Address  */
};

#define ARP_OPCODE_REQUEST     0x0001
#define ARP_OPCODE_REPLY       0x0002
#define ARP_PKT_OPCODE_REQUEST 0x0100		// To pass in 2 values we use a ushort
#define ARP_PKT_OPCODE_REPLY   0x0200 		// that takes the reverse of what we want

#define ZEROS		       0x00
#define ARP_LAN_HTYPE          0x0100		// LAN protocol for htype
#define ARP_NETLAY_PROTO       0x0008           // Network layer protocol 1st half
#define ARP_HLEN               0x06
#define ARP_PLEN               0x04

struct arpentry   //arp table entry variables 
{
	uchar ipAddress[IP_ADDR_LEN];
	uchar macAddress[ETH_ADDR_LEN];
	int state;
	int age;
	int pid;
};

#define ARPENT_LEN  10

struct arptable
{
	struct arpentry arps[ARPENT_LEN];
	semaphore arpsem;
	int arpnum; //number of entries in the arptable
};

extern struct arptable arptab; //arptable

/* ARP Table Entry States */
#define ARPENT_STATE_EMPTY         0
#define ARPENT_STATE_RESOLVED      1
#define ARPENT_STATE_UNRESOLVED    2

struct packet /* holds packets as well as interface and length */
{
	uchar payload[PKTSZ];
	uchar interface;
	int length;
};

#define ROUTEENT_LEN 10

struct routeEntry /*one route entry*/
{
	uchar destNetwork[IP_ADDR_LEN];
	uchar netmask[IP_ADDR_LEN];
	uchar gateway[IP_ADDR_LEN];
	int interface;
};

struct routeTable /*table for route entries*/
{
	struct routeEntry routes[ROUTEENT_LEN];
	semaphore routeSem;
	int routeNum;	
};

extern struct routeTable routeTab;

/* prototypes */
//net daemon prototype
void netDaemon(int);

//dhcp client prototype
void dhcpClient(int);

//arpDaemon prototype
void arpDaemon(void);

//icmpDaemon prototype
void icmpDaemon(void);

//route prototypes
void routeAdd(uchar*, uchar*, uchar*, int);
void routeInit(void);
int routeNextHop(uchar[]);

//my ip address
extern uchar myIP[IP_ADDR_LEN];

//flag to know if were looking for an ip
extern bool ipSet;

//various packet sends
void sendDiscoverPacket(void);
void sendArpResolvePacket(uchar*);
void sendArpReply(uchar[]);
void sendEchoRequestPacket(uchar[], uchar[], int, int);
void sendEchoReplyPacket(uchar[]);

#endif /*_NET_H_*/
