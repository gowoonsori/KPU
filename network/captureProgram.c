/*
    공통 수집 :   Ehternet header
                 IP header

    기본 start :  TCP / UDP / ICMP 3종류의 tcp/ip프로토콜만 수집

    필터 입력 가능 정보 :  port와 ip로만 추출 가능
                        -port :
                            HTTP(80)
                            DNS (53)
                         두 종류의 프로토콜만 추출 가능
                        -ip
*/
#include <arpa/inet.h>         //network 정보 변환
#include <ctype.h>             // isdigit
#include <netinet/if_ether.h>  //etherrnet 구조체
#include <netinet/ip.h>        //ip header 구조체
#include <netinet/ip_icmp.h>   //icmp header 구조체
#include <netinet/tcp.h>       //tcp header 구조체
#include <netinet/udp.h>       //udp header 구조체
#include <pthread.h>           //thread
#include <stdio.h>             //basic
#include <stdlib.h>            //malloc 동적할당
#include <string.h>            //strlen, strcmp, strcpy
#include <sys/socket.h>        //소켓의 주소 설정 (sockaddr 구조체)
#include <time.h>              //저장한 file 이름을 현재 날짜로 하기 위해

#define BUFFER_SIZE 65536  // buffer 사이즈 2^16 크기만큼 생성

typedef enum { false, true } bool;  // bool 자료형 선언

enum port { dns = 53, http = 80 };

void *PacketCapture_thread(void *arg);  //캡쳐 스레드

void Capture_helper(FILE *captureFile, unsigned char *, int);           //캡쳐한 패킷 프로토콜 분류
void Ethernet_header_capture(FILE *captureFile, unsigned char *, int);  // Ethernet 헤더 정보 fprint
void Ip_header_capture(FILE *captureFile, struct iphdr *, struct sockaddr_in,
                       struct sockaddr_in);                         // ip 헤더 정보 fprint
void Tcp_header_capture(FILE *captureFile, unsigned char *, int);   // tcp 헤더 정보 fprint
void Udp_header_capture(FILE *captureFile, unsigned char *, int);   // udp 헤더 정보 fprint
void Icmp_header_capture(FILE *captureFile, unsigned char *, int);  // icmp 헤더 정보 fprint
void Change_hex_to_ascii(FILE *captureFile, unsigned char *, int);  // payload hex값과 ascii 변환 후 fprint

void MenuBoard();           // menu board
void Menu_helper();         // menu exception handling
void StartMenuBoard();      // start menu board (filter 사용법 설명)
bool start_helper(char *);  // start menu exception handling | 제대로 된 filter 형식 : ture, 아니면 false

bool IsPort(char *);       // port번호 형식에 맞는지 검사 | 맞으면 true, 틀리면 false
bool IsIpAddress(char *);  // ip주소 형식에 맞는지 검사 | 맞으면 true, 틀리면 false
bool IsDigit();            //문자열이 숫자인지 검사 | 맞으면 true, 틀리면 false
void buffer_flush();       //입력 버퍼 지우기

bool captureStart = false;                                 //캡쳐 스레드 시작flag 변수
int total = 0, filter = 0, drop = 0;                       //캡쳐한 패킷 갯수
char protocolOption[128], portOption[128], ipOption[128];  // filter 옵션 변수

int main() {
    Menu_helper();

    return 0;
}

void *PacketCapture_thread(void *arg) {
    int rawSocket = *(int *)arg;                                   // raw socket 전달 받기
    int dataSize;                                                  //받은 데이터 정보 크기
    unsigned char *buffer = (unsigned char *)malloc(BUFFER_SIZE);  // buffer 공간 할당

    /*파일 open 및 파일 이름 현재 날짜로 생성*/
    char filename[40];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(filename, "captureFile(%d-%d-%d).txt", tm.tm_year - 100, tm.tm_mon + 1, tm.tm_mday);
    FILE *captureData = fopen(filename, "a+");  //파일 이어서 작성
    if (captureData == NULL) {
        printf("  !! 파일 열기 실패 하여 종료됩니다.\n");  //에러 처리
        exit(1);
    }

    //캡쳐 시작
    while (captureStart) {
        if ((dataSize = recvfrom(rawSocket, buffer, BUFFER_SIZE, 0, NULL, NULL)) == -1)  //패킷 recv
        {
            drop++;
            printf("packet 받기 실패\n");  // packet drop시 drop count
            continue;
        }
        Capture_helper(captureData, buffer, dataSize);  //받은 패킷을 filter규칙에 맞게 프로토콜 종류에따라 처리
    }

    free(buffer);         //버퍼 공간 해제
    fclose(captureData);  //소켓 close
}

void Capture_helper(FILE *captureData, unsigned char *buffer, int size) {
    struct iphdr *ipHeader = (struct iphdr *)(buffer + ETH_HLEN);  //받은 패킷의 ip header 부분 get
    total++;                                                       // recv한 패킷 총 갯수 증가

    /*모든 프로토콜 검색*/
    if (!strcmp(protocolOption, "*")) {
        switch (ipHeader->protocol)  // ip Header에 들어있는 프로토콜 종류 검사
        {
            case 1:  // ICMP
                Icmp_header_capture(captureData, buffer, size);
                break;
            case 6:  // TCP
                Tcp_header_capture(captureData, buffer, size);
                break;

            case 17:  // UDP
                Udp_header_capture(captureData, buffer, size);
                break;

            default:  // 그 외 프로토콜들은 무시
                break;
        }
    } else if (!strcmp(protocolOption, "tcp") && (ipHeader->protocol == 6)) {  // tcp 검색
        Tcp_header_capture(captureData, buffer, size);
    } else if (!strcmp(protocolOption, "udp") && (ipHeader->protocol == 17)) {  // udp 검색
        Udp_header_capture(captureData, buffer, size);
    } else if (!strcmp(protocolOption, "icmp") && (ipHeader->protocol == 1)) {  // icmp 검색
        Icmp_header_capture(captureData, buffer, size);
    }
}
void Ethrenet_header_capture(FILE *captureData, unsigned char *Buffer) {
    filter++;  // tcp / udp /icmp 에서 필터로 걸러진후 이 함수 호출하므로 필터링한 패킷 갯수 증가

    struct ethhdr *etherHeader = (struct ethhdr *)Buffer;  // 받아온 로우 데이터를 이더넷 헤더구조체 형태로 사용

    fprintf(captureData, "\n           --------------------------------------------------------\n");
    fprintf(captureData, "          |                     Ethernet Header                    |\n");
    fprintf(captureData, "           --------------------------------------------------------\n");
    fprintf(captureData, "               Ethernet Type         |      0x%02X00\n",
            etherHeader->h_proto);  // L3 패킷 타입 IPv4 : 0x0800  | ARP 패킷 : 0x0806 | VLAN Tag : 0x8100
    fprintf(captureData,
            "               Src MAC Addr          |      [%02x:%02x:%02x:%02x:%02x:%02x]\n",  // 6 byte for src
            etherHeader->h_source[0], etherHeader->h_source[1], etherHeader->h_source[2], etherHeader->h_source[3],
            etherHeader->h_source[4], etherHeader->h_source[5]);
    fprintf(captureData,
            "               Dst MAC Addr          |      [%02x:%02x:%02x:%02x:%02x:%02x]\n",  // 6 byte for dest
            etherHeader->h_dest[0], etherHeader->h_dest[1], etherHeader->h_dest[2], etherHeader->h_dest[3],
            etherHeader->h_dest[4], etherHeader->h_dest[5]);
    fprintf(captureData, "           --------------------------------------------------------\n\n");
}

void Ip_header_capture(FILE *captureData, struct iphdr *ipHeader, struct sockaddr_in source, struct sockaddr_in dest) {
    fprintf(captureData, "\n           --------------------------------------------------------\n");
    fprintf(captureData, "          |                       IP Header                        |\n");
    fprintf(captureData, "           --------------------------------------------------------\n");
    fprintf(captureData, "                IP Version           |    IPv%d\n", (unsigned int)ipHeader->version);
    fprintf(captureData, "                IP Header Length     |    %d DWORDS ( %d Bytes )\n",
            (unsigned int)ipHeader->ihl, ((unsigned int)(ipHeader->ihl)) * 4);
    fprintf(captureData, "                Type Of Service      |    %d\n", (unsigned int)ipHeader->tos);
    fprintf(captureData, "                IP Total Length      |    %d Bytes\n", ntohs(ipHeader->tot_len));
    fprintf(captureData, "           --------------------------------------------------------\n");
    fprintf(captureData, "                Identification       |    %d\n", ntohs(ipHeader->id));
    fprintf(captureData, "           --------------------------------------------------------\n");
    fprintf(captureData, "                Time To Live (TTL)   |    %d\n", (unsigned int)ipHeader->ttl);
    fprintf(captureData, "                Protocol             |    %d\n", (unsigned int)ipHeader->protocol);
    fprintf(captureData, "                Checksum             |    0x%04X\n", ntohs(ipHeader->check));
    fprintf(captureData, "           --------------------------------------------------------\n");
    fprintf(captureData, "                Src IP Addr          |    %s\n", inet_ntoa(source.sin_addr));
    fprintf(captureData, "                Dst IP Addr          |    %s\n", inet_ntoa(dest.sin_addr));
    fprintf(captureData, "           --------------------------------------------------------\n\n");
}

void Tcp_header_capture(FILE *captureData, unsigned char *Buffer, int Size) {
    struct iphdr *ipHeader = (struct iphdr *)(Buffer + ETH_HLEN);  //버퍼(캡쳐한 패킷데이터) 에서 ip헤더 정보 get
    struct tcphdr *tcph = (struct tcphdr *)(Buffer + (ipHeader->ihl * 4) + ETH_HLEN);  //버퍼에서 tcp 헤더 정보 get
    struct sockaddr_in source, dest;  //출발, 목적지 주소 정보 저장
    source.sin_addr.s_addr = ipHeader->saddr;
    dest.sin_addr.s_addr = ipHeader->daddr;

    // filter ip 검사
    if (!strcmp(ipOption, "*") || !strcmp(inet_ntoa(source.sin_addr), ipOption) ||
        !strcmp(inet_ntoa(dest.sin_addr), ipOption)) {
        // filter port번호 검사
        if (!strcmp(portOption, "*") || (atoi(portOption) == (int)ntohs(tcph->source)) ||
            (atoi(portOption) == (int)ntohs(tcph->dest))) {
            fprintf(captureData, "\n############################## TCP Packet #####################################\n");
            Ethrenet_header_capture(captureData, Buffer);            // ethernet 정보 print
            Ip_header_capture(captureData, ipHeader, source, dest);  // ip 정보 print

            time_t t = time(NULL);
            struct tm tm = *localtime(&t);
            fprintf(stdout, "\n%d:%d:%06d IPv", tm.tm_hour, tm.tm_min, tm.tm_sec);
            if (ntohs(tcph->source) == http) {
                fprintf(stdout, "%d %s:http > ", (unsigned int)ipHeader->version, inet_ntoa(source.sin_addr));
                fprintf(stdout, "%s:%u = TCP Flags [", inet_ntoa(dest.sin_addr), ntohs(tcph->dest));
            } else if (ntohs(tcph->dest) == http) {
                fprintf(stdout, "%d %s:%u > ", (unsigned int)ipHeader->version, inet_ntoa(source.sin_addr),
                        ntohs(tcph->source));
                fprintf(stdout, "%s:http = TCP Flags [", inet_ntoa(dest.sin_addr));
            } else {
                fprintf(stdout, "%d %s:%u > ", (unsigned int)ipHeader->version, inet_ntoa(source.sin_addr),
                        ntohs(tcph->source));
                fprintf(stdout, "%s:%u = TCP Flags [", inet_ntoa(dest.sin_addr), ntohs(tcph->dest));
            }
            fprintf(captureData, "\n           --------------------------------------------------------\n");
            fprintf(captureData, "          |                       TCP Header                       |\n");
            fprintf(captureData, "           --------------------------------------------------------\n");
            fprintf(captureData, "             Source Port             |   %u\n", ntohs(tcph->source));
            fprintf(captureData, "             Dest Port               |   %u\n", ntohs(tcph->dest));
            fprintf(captureData, "           --------------------------------------------------------\n");
            fprintf(captureData, "             Sequence Number         |   %u\n", ntohl(tcph->seq));
            fprintf(captureData, "             Acknowledge Number      |   %u\n", ntohl(tcph->ack_seq));
            fprintf(captureData, "           --------------------------------------------------------\n");
            fprintf(captureData, "             OFFSET(Header Length)   |   %d DWORDS (%d BYTES)\n",
                    (unsigned int)tcph->doff, (unsigned int)tcph->doff * 4);
            fprintf(captureData, "           -- FLAGS -----------------------------------------------\n");
            fprintf(captureData, "              |-Urgent Flag          |   %d\n", (unsigned int)tcph->urg);
            if ((unsigned int)tcph->urg == 1) fprintf(stdout, "U.");
            fprintf(captureData, "              |-Ack Flag             |   %d\n", (unsigned int)tcph->ack);
            if ((unsigned int)tcph->ack == 1) fprintf(stdout, "A.");
            fprintf(captureData, "              |-Push Flag            |   %d\n", (unsigned int)tcph->psh);
            if ((unsigned int)tcph->psh == 1) fprintf(stdout, "P.");
            fprintf(captureData, "              |-Reset Flag           |   %d\n", (unsigned int)tcph->rst);
            if ((unsigned int)tcph->rst == 1) fprintf(stdout, "R.");
            fprintf(captureData, "              |-Synchronise Flag     |   %d\n", (unsigned int)tcph->syn);
            if ((unsigned int)tcph->syn == 1) fprintf(stdout, "S.");
            fprintf(captureData, "              |-Finish Flag          |   %d\n", (unsigned int)tcph->fin);
            if ((unsigned int)tcph->fin == 1) fprintf(stdout, "F.");
            fprintf(captureData, "             Window Size (rwnd)      |   %d\n", ntohs(tcph->window));
            fprintf(captureData, "           --------------------------------------------------------\n");
            fprintf(captureData, "             Checksum                |   0x%04x\n", ntohs(tcph->check));
            fprintf(captureData, "             Urgent Pointer          |   %d\n", tcph->urg_ptr);
            fprintf(captureData, "           --------------------------------------------------------\n");
            fprintf(stdout, "], seq %u, ack %u, win %d, length %d", ntohl(tcph->seq), ntohl(tcph->ack_seq),
                    ntohs(tcph->window), Size);

            /* 패킷 정보 Hex dump 와 ASCII 변환 데이터 출력 */
            Change_hex_to_ascii(captureData, Buffer + ETH_HLEN + (ipHeader->ihl * 4) + tcph->doff * 4,
                                (Size - tcph->doff * 4 - (ipHeader->ihl * 4) - ETH_HLEN));
            fprintf(captureData, "\n===============================================================================\n");
        }
    }
}

void Udp_header_capture(FILE *captureData, unsigned char *Buffer, int Size) {
    struct iphdr *ipHeader = (struct iphdr *)(Buffer + ETH_HLEN);  //버퍼(캡쳐한 패킷데이터) 에서 ip헤더 정보 get
    struct udphdr *udph = (struct udphdr *)(Buffer + ipHeader->ihl * 4 + ETH_HLEN);  //버퍼에서 udp 헤더 정보 get
    struct sockaddr_in source, dest;  //출발, 목적지 주소 정보 저장
    source.sin_addr.s_addr = ipHeader->saddr;
    dest.sin_addr.s_addr = ipHeader->daddr;

    // ip filter 검사
    if (!strcmp(ipOption, "*") || !strcmp(inet_ntoa(source.sin_addr), ipOption) ||
        !strcmp(inet_ntoa(dest.sin_addr), ipOption)) {
        // port 번호 filter 검사
        if (!strcmp(portOption, "*") || (atoi(portOption) == (int)ntohs(udph->source)) ||
            (atoi(portOption) == (int)ntohs(udph->dest))) {
            fprintf(captureData, "\n############################## UDP Packet #####################################\n");
            Ethrenet_header_capture(captureData, Buffer);            // ethernet 정보 print
            Ip_header_capture(captureData, ipHeader, source, dest);  // ip 정보 print

            time_t t = time(NULL);
            struct tm tm = *localtime(&t);
            fprintf(stdout, "\n%d:%d:%06d IPv", tm.tm_hour, tm.tm_min, tm.tm_sec);
            if (ntohs(udph->source) == dns) {
                fprintf(stdout, "%d %s:dns > ", (unsigned int)ipHeader->version, inet_ntoa(source.sin_addr));
                fprintf(stdout, "%s:%u = UDP ", inet_ntoa(dest.sin_addr), ntohs(udph->dest));
            } else if (ntohs(udph->dest) == dns) {
                fprintf(stdout, "%d %s:%u > ", (unsigned int)ipHeader->version, inet_ntoa(source.sin_addr),
                        ntohs(udph->source));
                fprintf(stdout, "%s:dns = UDP ", inet_ntoa(dest.sin_addr));
            } else {
                fprintf(stdout, "%d %s:%u > ", (unsigned int)ipHeader->version, inet_ntoa(source.sin_addr),
                        ntohs(udph->source));
                fprintf(stdout, "%s:%u = UDP ", inet_ntoa(dest.sin_addr), ntohs(udph->dest));
            }
            fprintf(captureData, "\n           --------------------------------------------------------\n");
            fprintf(captureData, "          |                       UDP Header                       |\n");
            fprintf(captureData, "           --------------------------------------------------------\n");
            fprintf(captureData, "                Source Port          |   %u\n", ntohs(udph->source));
            fprintf(captureData, "                Destination Port     |   %u\n", ntohs(udph->dest));
            fprintf(captureData, "           --------------------------------------------------------\n");
            fprintf(captureData, "                UDP Length           |   %d\n", ntohs(udph->len));
            fprintf(captureData, "                UDP Checksum         |   0x%04x\n", ntohs(udph->check));
            fprintf(captureData, "           --------------------------------------------------------\n");
            fprintf(stdout, "length %d", Size);

            /* 패킷 정보 Hex dump 와 ASCII 변환 데이터 출력 */
            Change_hex_to_ascii(captureData, Buffer + (ipHeader->ihl * 4) + sizeof udph,
                                (Size - sizeof udph - (ipHeader->ihl * 4) - ETH_HLEN));
            fprintf(captureData, "\n===============================================================================\n");
        }
    }
}

void Icmp_header_capture(FILE *captureData, unsigned char *Buffer, int Size) {
    struct iphdr *ipHeader = (struct iphdr *)(Buffer + ETH_HLEN);  //버퍼(캡쳐한 패킷데이터) 에서 ip헤더 정보 get
    struct icmphdr *icmph = (struct icmphdr *)(Buffer + ipHeader->ihl * 4 + ETH_HLEN);  //버퍼에서 icp정보 get
    struct sockaddr_in source, dest;  //출발, 목적지 주소 정보 저장
    source.sin_addr.s_addr = ipHeader->saddr;
    dest.sin_addr.s_addr = ipHeader->daddr;

    // ip filter 검사
    if (!strcmp(ipOption, "*") || !strcmp(inet_ntoa(source.sin_addr), ipOption) ||
        !strcmp(inet_ntoa(dest.sin_addr), ipOption)) {
        fprintf(captureData, "\n############################## ICMP Packet ####################################\n");
        Ethrenet_header_capture(captureData, Buffer);            // ethernet 정보 print
        Ip_header_capture(captureData, ipHeader, source, dest);  // ip 정보 print

        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        fprintf(stdout, "\n%d:%d:%06d IPv%d %s > ", tm.tm_hour, tm.tm_min, tm.tm_sec, (unsigned int)ipHeader->version,
                inet_ntoa(source.sin_addr));
        fprintf(stdout, "%s = ICMP ", inet_ntoa(dest.sin_addr));
        fprintf(captureData, "\n           --------------------------------------------------------\n");
        fprintf(captureData, "          |                      ICMP Header                       |\n");
        fprintf(captureData, "           --------------------------------------------------------\n");
        fprintf(captureData, "                  Type             |   %d", (unsigned int)(icmph->type));
        fprintf(captureData, "           --------------------------------------------------------\n");
        fprintf(captureData, "                  Code             |   %d\n", (unsigned int)(icmph->code));
        fprintf(captureData, "                  Checksum         |   0x%04x\n", ntohs(icmph->checksum));
        fprintf(captureData, "           --------------------------------------------------------\n");
        fprintf(stdout, "length %d", Size);
        /* 패킷 정보 Hex dump 와 ASCII 변환 데이터 출력 */
        Change_hex_to_ascii(captureData, Buffer + ETH_HLEN + (ipHeader->ihl * 4) + sizeof icmph,
                            (Size - sizeof icmph - (ipHeader->ihl * 4) - ETH_HLEN));
        fprintf(captureData, "\n===============================================================================\n");
    }
}

void Change_hex_to_ascii(FILE *captureData, unsigned char *data, int Size) {
    fprintf(captureData, "\n\nDATA (Payload)\n");
    for (int i = 0; i < Size; i++) {
        if (i != 0 && i % 16 == 0) {  // 16개 데이터 출력 했다면, ascii코드 출력후 개행후 이어서 출력
            fprintf(captureData, "\t\t");     // 16진수 data랑 ascii data 구분
            for (int j = i - 16; j < i; j++)  // 16진수 data를 ascii로 변환
            {
                if (data[j] >= 32 && data[j] < 128)
                    fprintf(captureData, "%c", (unsigned char)data[j]);  // data가 ascii라면 출력

                else
                    fprintf(captureData, ".");  //그외 데이터는 . 으로 표현
            }
            fprintf(captureData, "\n");
        }

        if (i % 16 == 0) fprintf(captureData, "\t");  //가시성을 위해 처음 오는 data는 tab

        fprintf(captureData, " %02X", (unsigned int)data[i]);  //앞의 빈자리 0으로 초기화한 16진수로 데이터 출력

        if (i == Size - 1)  //마지막 data
        {
            for (int j = 0; j < (15 - (i % 16)); j++)
                fprintf(captureData,
                        "   ");  //마지막 데이터는 16개 꽉 안채울 수 있으니 데이터 포맷을 위해 남은 공간만큼 space

            fprintf(captureData, "\t\t");  // 16자리 까지 공백 채운후 ascii 출력 위해 구분

            for (int j = (i - (i % 16)); j <= i; j++)  //남은 데이터 ascii로 변환
            {
                if (data[j] >= 32 && data[j] < 128)
                    fprintf(captureData, "%c", (unsigned char)data[j]);
                else
                    fprintf(captureData, ".");
            }
            fprintf(captureData, "\n");
        }
    }
}

void MenuBoard() {
    system("clear");
    fprintf(stdout, "\n************************** WELCOME ************************\n");
    fprintf(stdout, "*                    Custom Packet Capture                *\n");
    fprintf(stdout, "**************************** Menu *************************\n\n");
    fprintf(stdout, "                     1. Capture start \n");
    fprintf(stdout, "                     2. Capture stop \n");
    fprintf(stdout, "                     3. show menu \n");
    fprintf(stdout, "                     0. exit \n");
    fprintf(stdout, " \n**********************************************************\n\n");
}

void StartMenuBoard() {
    system("clear");
    fprintf(stdout, "\n********************** 캡쳐 가능 프로토콜 *******************\n\n");
    fprintf(stdout, "                    *(all) | tcp | udp | icmp \n");
    fprintf(stdout, "\n*********************** port 번호 rule **********************\n\n");
    fprintf(stdout, "        *(all) | 0 ~ 65535 | [http(80) | dns(53) | icmp(*)]  \n");
    fprintf(stdout, "\n********************** ip 주소 입력 rule ********************\n\n");
    fprintf(stdout, "                 *(all) | 0.0.0.0 ~ 255.255.255.255 \n");
    fprintf(stdout, "\n************************* Start Rule ************************\n\n");
    fprintf(stdout, "                 입력 :  프로토콜 포트번호 ip주소  \n");
    fprintf(stdout, "\n*************************************************************\n\n");
}

void Menu_helper() {
    int isDigit, menuItem;  // menu판 입력 변수  ( isDigit = 1:숫자  false: 숫자아님 / menuItem : 메뉴번호)
    pthread_t capture_thd;  //패킷 캡쳐 스레드
    int rawSocket;          // raw socket
    char str[128];

    if ((rawSocket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1)  // raw socket 생성
    {
        printf("Socket 열기 실패\n");
        exit(1);
    }

    //프로그램 종료시까지 반복
    MenuBoard();
    while (1) {
        fprintf(stdout, "\n   메뉴 번호 입력 : ");
        isDigit = scanf("%d", &menuItem);  //메뉴판 번호 입력
        buffer_flush();                    //입력버퍼 flush

        if (menuItem == 0 && isDigit == 1)  //프로그램 종료
        {
            fprintf(stderr, "   !!! Good bye !!!");
            break;
        } else if (menuItem == 1 && isDigit == 1)  // 캡쳐 시작
        {
            //시작 중에 또 시작버튼 클릭시 error
            if (captureStart)
                fprintf(stdout, "이미 시작 중입니다 !!\n");
            else {
                StartMenuBoard();
                fprintf(stdout, "\n   필터 입력 : ");
                scanf("%[^\n]s", str);

                if (start_helper(str)) {  // filter exception handling
                    captureStart = true;
                    pthread_create(&capture_thd, NULL, PacketCapture_thread,
                                   (void *)&rawSocket);  // TCP 캡쳐 스레드 생성
                    pthread_detach(capture_thd);         //스레드 종료시 자원 해제
                }
            }
        } else if (menuItem == 2 && isDigit == 1)  // 캡쳐 중지
        {
            if (!captureStart)
                fprintf(stdout, "시작 중이 아닙니다 !!\n");
            else {
                captureStart = false;
                fprintf(stdout, "\n\n캡쳐 중지.\n");
                fprintf(stdout, "%d packets captured\n", total);
                fprintf(stdout, "%d filtered packets captured\n", filter);
                fprintf(stdout, "%d packets dropped\n", drop);
                total = 0, filter = 0, drop = 0;
            }
        } else if (menuItem == 3 && isDigit == 1)  // show Menu
        {
            MenuBoard();
        } else {  //메뉴 번호 exception handling
            fprintf(stderr, "잘못 입력하셨습니다 !!\n\n");
        }
    }
    close(rawSocket);  // socket close
}
bool start_helper(char *str) {
    char *option = strtok(str, " ");  //프로토콜 option
    if (strcmp(option, "*") && strcmp(option, "tcp") && strcmp(option, "udp") && strcmp(option, "icmp") &&
        strcmp(option, "http") && strcmp(option, "dns")) {
        fprintf(stderr, "* | tcp | udp | icmp | http | dns 만 캡쳐가능합니다.\n");
        return false;
    }
    strcpy(protocolOption, option);

    option = strtok(NULL, " ");  // port번호 option
    if (!IsPort(option)) {
        fprintf(stderr, "잘못된 port 입력입니다.\n");
        return false;
    }
    strcpy(portOption, option);

    option = strtok(NULL, "\n");  // ip 주소 option
    char s[48];
    strcpy(s, option);
    if (!IsIpAddress(s)) {
        fprintf(stderr, "잘못된 Ip 주소 입니다.\n");
        return false;
    }
    strcpy(ipOption, option);

    return true;
}

bool IsPort(char *str) {
    if (!IsDigit(str))  //숫자가 아니라면 flase
        return false;
    if (atoi(str) < 1 || atoi(str) > 65535 || !strcmp(str, "*"))  //없는 포트번호거나 *가 아니라면 false
        return false;
    return true;
}

bool IsIpAddress(char *str) {
    if (!strcmp(str, "*"))  //모든 ip주소 filter
        return true;
    int numberOfOctet = 0;

    char *octet = strtok(str, ".");  // ip octet 규칙 검사
    while (octet != NULL) {
        if ((!isdigit(octet[0]) && atoi(octet) == 0) || atoi(octet) > 255)  //알파벳이거나 255를 넘는다면 false
        {
            return false;
        }
        numberOfOctet++;
        octet = strtok(NULL, ".");
    }
    if (numberOfOctet != 4)  // octet 이 4개가 안된다면 false
        return false;

    return true;
}

void buffer_flush() {
    while (getchar() != '\n')
        ;
}
bool IsDigit(char *str) {
    for (int i = 0; i < (int)strlen(str); i++) {
        if (!isdigit(str[i]))  //숫자아니라면 false
            return false;
    }
    return true;
}