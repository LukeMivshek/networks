/*HW Assignment #1 by Casey French & Luke Mivshek*/

BYTES     | DESCRIPTION		   	   | HEX

Ethernet Frame
0 - 5     | Destination Address     	   | ff ff ff ff ff ff
6 - 11    | Source Address  	   	   | 00 1e e5 86 02 7a
12 - 13   | Type 	 	   	   | 08 00

IPv4 Header
14	  | Version & Header Length        | 4 & 5 (5 x 4 = 20 bytes)
15  	  | Service Type	           | 00
16 - 17   | Total Length	           | 01 1a
18 - 19   | Identification	   	   | 0003
20 - 21   | Flags & Fragmentation offset   | 0 & 000 (3 & 13, 000 and 000000000) 
22 	  | Time-to-live		   | 64
23	  | Protocol			   | 11
24 - 25	  | Header checksum		   | 55 d1
26 - 29   | Source IP Address		   | 00 00 00 00
30 - 33   | Destination IP Address	   | ff ff ff ff

UDP Header
34 - 35   | Source Port Number		   | 00 44
36 - 37   | Destination Port Number	   | 00 43
38 - 39   | Total Length		   | 01 06
40 - 41   | Checksum			   | 6e 71

DHCP Header
42	  | Opcode			   | 01 
43	  | Htype		   	   | 01
44	  | HLen			   | 06
45	  | HCount			   | 00
46 - 49   | Transaction ID		   | 00 00 00 17
50 - 51	  | Time elapsed		   | 00 00
52 - 53   | Flags			   | 00 00
54 - 57   | Client IP Address		   | 00 00 00 00
58 - 61   | Your IP Address		   | 00 00 00 00
62 - 65   | Server IP Address		   | 00 00 00 00
66 - 69   | Gateway IP Address		   | 00 00 00 00
70 - 85   | Client Hardware Address	   | 00 1e e5 86 02 7a 00 00 00 00 00 00 00 00 00 00
86 - 149  | Server Name			   | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
					   | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
					   | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
					   | 00 00 00 00 00 00 00 00 00 00
150 - 277 | Boot File Name		   | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
					   | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
					   | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
					   | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
					   | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
					   | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
					   | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
					   | 00 00
DHCP Options
278 - 281 | Magic Cookie		   | 63 82 53 63
282	  | Tag				   | 35 (53 decimal, DHCP Message Type)
283	  | Length			   | 01 (1 decimal)
284	  | Value			   | 01 (1 decimal, DHCPDISCOVER)
285	  | Tag				   | 37 (55 decimal, Parameter Request List)
286	  | Length			   | 08 
287 - 295 | Option Codes		   | 01 0f 03 06 10 11 82 93
296	  | End				   | ff
