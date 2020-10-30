#include <arpa/inet.h>         //network 정보 변환
#include <netinet/if_ether.h>  //etherrnet 구조체
#include <netinet/ip.h>        //ip header 구조체
#include <netinet/ip_icmp.h>   //icmp header 구조체
#include <netinet/tcp.h>       //tcp header 구조체
#include <netinet/udp.h>       //udp header 구조체
#include <pthread.h>           //thread
#include <stdio.h>             //basic
#include <stdlib.h>            //malloc 동적할당
#include <string.h>            //memset 초기화
#include <sys/socket.h>        //소켓의 주소 설정 (ip 주소로 변환하기 위한 sockaddr 구조체)
#include <time.h>              //저장한 file 이름을 현재 날짜로 하기 위해

#define BUFFER_SIZE 65536  // buffer 사이즈 최대 크기인 2^16 크기만큼 생성

typedef enum { false, true } bool;  // bool 자료형 선언

void *PacketCapture_thread(void *arg);  //캡쳐 스레드

void Ethernet_header_capture(FILE *captureFile, unsigned char *, int);  // Ethernet 헤더 정보 fprint
void Ip_header_capture(FILE *captureFile, unsigned char *);             // ip 헤더 정보 fprint
void Tcp_header_capture(FILE *captureFile, unsigned char *, int);       // tcp 헤더 정보 fprint
void Udp_header_capture(FILE *captureFile, unsigned char *, int);       // udp 헤더 정보 fprint
void Icmp_header_capture(FILE *captureFile, unsigned char *, int);      // icmp 헤더 정보 fprint
void Capture_helper(FILE *captureFile, unsigned char *, int);           //캡쳐한 패킷 프로토콜 분류

void Change_hex_to_ascii(FILE *captureFile, unsigned char *, int);  // payload hex값과 ascii 변환 후 fprint

void MenuBoard();  // menu board
void Menu_helper();
void buffer_flush();  //입력 버퍼 지우기

bool captureStart = false;  //캡쳐 스레드 시작flag 변수

int main() {
    Menu_helper();

    return 0;
}

void *PacketCapture_thread(void *arg) {
    int rawSocket = *(int *)arg;                                   // raw socket 전달 받기
    int dataSize;                                                  //받은 데이터 정보 크기
    unsigned char *buffer = (unsigned char *)malloc(BUFFER_SIZE);  // buffer 공간 할당

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
        if ((dataSize = recvfrom(rawSocket, buffer, BUFFER_SIZE, 0, NULL, NULL)) == -1) {  //패킷 recv
            printf("packet 받기 실패\n");                                                  // packet drop시
            continue;
        }
        Capture_helper(captureData, buffer, dataSize);  //받은 패킷을 프로토콜 종류에따라 처리
    }

    free(buffer);         //버퍼 공간 해제
    fclose(captureData);  //소켓 close
}

void Capture_helper(FILE *captureData, unsigned char *buffer, int size) {
    struct iphdr *ipHeader = (struct iphdr *)(buffer + ETH_HLEN);  //받은 패킷의 ip header 부분 get
    switch (ipHeader->protocol) {  // ip Header에 들어있는 프로토콜 종류 검사

        case 1:  // ICMP : 1번 포트
            Icmp_header_capture(captureData, buffer, size);
            break;

        case 6:  // TCP : 6번 포트
            Tcp_header_capture(captureData, buffer, size);
            break;

        case 17:  // UDP : 17번 포트
            Udp_header_capture(captureData, buffer, size);
            break;

        default:  // 그 외 프로토콜들은 무시
            break;
    }
}
void Ethrenet_header_capture(FILE *captureData, unsigned char *Buffer) {
    struct ethhdr *etherHeader = (struct ethhdr *)Buffer;  // 받아온 로우 데이터를 이더넷 헤더구조체 형태로 사용

    fprintf(captureData, "\n           --------------------------------------------------------\n");
    fprintf(captureData, "          |                     Ethernet Header                    |\n");
    fprintf(captureData, "           --------------------------------------------------------\n");
    fprintf(captureData, "               Ethernet Type         |      %X\n",
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

void Ip_header_capture(FILE *captureData, unsigned char *Buffer) {
    struct iphdr *ipHeader = (struct iphdr *)Buffer;  // buffer(패킷 데이터)에서 ip 헤더 정보 get
    struct sockaddr_in source, dest;                  //출발, 목적지 주소 정보 저장할 변수

    source.sin_addr.s_addr = ipHeader->saddr;
    dest.sin_addr.s_addr = ipHeader->daddr;

    fprintf(captureData, "\n           --------------------------------------------------------\n");
    fprintf(captureData, "          |                       IP Header                        |\n");
    fprintf(captureData, "           --------------------------------------------------------\n");
    fprintf(captureData, "                IP Version           |    %d\n", (unsigned int)ipHeader->version);
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
    unsigned short ipHeaderLen = ipHeader->ihl * 4;                // ip 헤더 길이
    struct tcphdr *tcph = (struct tcphdr *)(Buffer + ipHeaderLen + ETH_HLEN);  //버퍼에서 tcp 헤더 정보 get

    fprintf(captureData, "\n############################## TCP Packet #####################################\n");
    Ethrenet_header_capture(captureData, Buffer);       // ethernet 정보 print
    Ip_header_capture(captureData, Buffer + ETH_HLEN);  // ip 정보 print

    fprintf(captureData, "\n           --------------------------------------------------------\n");
    fprintf(captureData, "          |                       TCP Header                       |\n");
    fprintf(captureData, "           --------------------------------------------------------\n");
    fprintf(captureData, "             Source Port             |   %u\n", ntohs(tcph->source));
    fprintf(captureData, "             Dest Port               |   %u\n", ntohs(tcph->dest));
    fprintf(captureData, "           --------------------------------------------------------\n");
    fprintf(captureData, "             Sequence Number         |   %u\n", ntohl(tcph->seq));
    fprintf(captureData, "             Acknowledge Number      |   %u\n", ntohl(tcph->ack_seq));
    fprintf(captureData, "           --------------------------------------------------------\n");
    fprintf(captureData, "             OFFSET(Header Length)   |   %d DWORDS (%d BYTES)\n", (unsigned int)tcph->doff,
            (unsigned int)tcph->doff * 4);
    fprintf(captureData, "           -- FLAGS -----------------------------------------------\n");
    fprintf(captureData, "              |-Urgent Flag          |   %d\n", (unsigned int)tcph->urg);
    fprintf(captureData, "              |-Ack Flag             |   %d\n", (unsigned int)tcph->ack);
    fprintf(captureData, "              |-Push Flag            |   %d\n", (unsigned int)tcph->psh);
    fprintf(captureData, "              |-Reset Flag           |   %d\n", (unsigned int)tcph->rst);
    fprintf(captureData, "              |-Synchronise Flag     |   %d\n", (unsigned int)tcph->syn);
    fprintf(captureData, "              |-Finish Flag          |   %d\n", (unsigned int)tcph->fin);
    fprintf(captureData, "             Window Size (rwnd)      |   %d\n", ntohs(tcph->window));
    fprintf(captureData, "           --------------------------------------------------------\n");
    fprintf(captureData, "             Checksum                |   0x%04x\n", ntohs(tcph->check));
    fprintf(captureData, "             Urgent Pointer          |   %d\n", tcph->urg_ptr);
    fprintf(captureData, "           --------------------------------------------------------\n");

    /* 패킷 정보 Hex dump 와 ASCII 변환 데이터 출력 */
    fprintf(captureData, "\n\nDATA (Payload)\n");
    Change_hex_to_ascii(captureData, Buffer + ETH_HLEN + ipHeaderLen + tcph->doff * 4,
                        (Size - tcph->doff * 4 - ipHeaderLen - ETH_HLEN));

    fprintf(captureData, "\n===============================================================================\n");
}

void Udp_header_capture(FILE *captureData, unsigned char *Buffer, int Size) {
    struct iphdr *ipHeader = (struct iphdr *)(Buffer + ETH_HLEN);  //버퍼(캡쳐한 패킷데이터) 에서 ip헤더 정보 get
    unsigned short ipHeaderLen = ipHeader->ihl * 4;                // ip 헤더 길이 저장
    struct udphdr *udph = (struct udphdr *)(Buffer + ipHeaderLen + ETH_HLEN);  //버퍼에서 udp 헤더 정보 get

    fprintf(captureData, "\n############################## UDP Packet #####################################\n");

    Ethrenet_header_capture(captureData, Buffer);       // ethernet 정보 print
    Ip_header_capture(captureData, Buffer + ETH_HLEN);  // ip 헤더 print

    fprintf(captureData, "\n           --------------------------------------------------------\n");
    fprintf(captureData, "          |                       UDP Header                       |\n");
    fprintf(captureData, "           --------------------------------------------------------\n");
    fprintf(captureData, "                Source Port          |   %u\n", ntohs(udph->source));
    fprintf(captureData, "                Destination Port     |   %u\n", ntohs(udph->dest));
    fprintf(captureData, "           --------------------------------------------------------\n");
    fprintf(captureData, "                UDP Length           |   %d\n", ntohs(udph->len));
    fprintf(captureData, "                UDP Checksum         |   0x%04x\n", ntohs(udph->check));
    fprintf(captureData, "           --------------------------------------------------------\n");

    /* 패킷 정보 Hex dump 와 ASCII 변환 데이터 출력 */
    fprintf(captureData, "\n\nDATA (Payload)\n");
    Change_hex_to_ascii(captureData, Buffer + ipHeaderLen + sizeof udph, (Size - sizeof udph - ipHeaderLen - ETH_HLEN));

    fprintf(captureData, "\n===============================================================================\n");
}

void Icmp_header_capture(FILE *captureData, unsigned char *Buffer, int Size) {
    struct iphdr *ipHeader = (struct iphdr *)(Buffer + ETH_HLEN);  //버퍼(캡쳐한 패킷데이터) 에서 ip헤더 정보 get
    unsigned short ipHeaderLen = ipHeader->ihl * 4;                // ip 헤더 길이 저장
    struct icmphdr *icmph = (struct icmphdr *)(Buffer + ipHeaderLen + ETH_HLEN);  //버퍼에서 icp정보 get

    fprintf(captureData, "\n############################## ICMP Packet ####################################\n");
    Ethrenet_header_capture(captureData, Buffer);       // ethernet 정보 print
    Ip_header_capture(captureData, Buffer + ETH_HLEN);  // ip 헤더 print

    fprintf(captureData, "\n           --------------------------------------------------------\n");
    fprintf(captureData, "          |                      ICMP Header                       |\n");
    fprintf(captureData, "           --------------------------------------------------------\n");
    fprintf(captureData, "                  Type             |   %d", (unsigned int)(icmph->type));
    fprintf(captureData, "           --------------------------------------------------------\n");
    fprintf(captureData, "                  Code             |   %d\n", (unsigned int)(icmph->code));
    fprintf(captureData, "                  Checksum         |   0x%04x\n", ntohs(icmph->checksum));
    fprintf(captureData, "           --------------------------------------------------------\n");

    /* 패킷 정보 Hex dump 와 ASCII 변환 데이터 출력 */
    fprintf(captureData, "\n\nDATA (Payload)\n");
    Change_hex_to_ascii(captureData, Buffer + ETH_HLEN + ipHeaderLen + sizeof icmph,
                        (Size - sizeof icmph - ipHeaderLen - ETH_HLEN));

    fprintf(captureData, "\n===============================================================================\n");
}

void Change_hex_to_ascii(FILE *captureData, unsigned char *data, int Size) {
    for (int i = 0; i < Size; i++) {
        if (i != 0 && i % 16 == 0) {  // 16개 데이터 출력 했다면, ascii코드 출력후 개행후 이어서 출력
            fprintf(captureData, "\t\t");       // 16진수 data랑 ascii data 구분
            for (int j = i - 16; j < i; j++) {  // 16진수 data를 ascii로 변환
                if (data[j] >= 32 && data[j] < 128)
                    fprintf(captureData, "%c", (unsigned char)data[j]);  // data가 ascii라면 출력
                else
                    fprintf(captureData, ".");  //그외 데이터는 . 으로 표현
            }
            fprintf(captureData, "\n");
        }

        if (i % 16 == 0) fprintf(captureData, "\t");  //가시성을 위해 처음 오는 data는 tab

        fprintf(captureData, " %02X",
                (unsigned int)data[i]);  //앞의 빈자리 0으로 초기화한 16진수로 데이터 출력

        if (i == Size - 1) {  //마지막 data
            for (int j = 0; j < (15 - (i % 16)); j++)
                fprintf(captureData, "   ");  //마지막 데이터는 데이터 포맷을 위해 남은 공간만큼 space
            fprintf(captureData, "\t\t");     // 16자리 까지 공백 채운후 ascii 출력 위해 구분

            for (int j = (i - (i % 16)); j <= i; j++) {  //남은 데이터 ascii로 변환
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

void Menu_helper() {
    int isDigit,
        menuItem;  // menu판 입력 변수  ( isDigit = 1:숫자  false: 숫자아님 / menuItem : 메뉴번호)
    pthread_t capture_thd;  //패킷 캡쳐 스레드
    int rawSocket;          // raw socket

    if ((rawSocket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1) {  // raw socket 생성
        printf("Socket 열기 실패\n");
        exit(1);
    }

    //프로그램 종료시까지 반복
    MenuBoard();
    while (1) {
        fprintf(stdout, "\n    입력 : ");
        isDigit = scanf("%d", &menuItem);  //메뉴판 번호 입력
        buffer_flush();                    //입력버퍼 flush

        if ((menuItem == 1 || menuItem == 2 || menuItem == 3) && isDigit == 1 && !captureStart)
            fprintf(stdout, "캡쳐중... (종료는 2)\n");

        if (menuItem == 0 && isDigit == 1) {  //프로그램 종료
            fprintf(stderr, "   !!! Good bye !!!");
            break;
        } else if (menuItem == 1 && isDigit == 1) {  // TCP 캡쳐 시작
            if (captureStart)
                fprintf(stdout, "이미 시작 중입니다 !!\n");
            else {
                captureStart = true;
                pthread_create(&capture_thd, NULL, PacketCapture_thread, (void *)&rawSocket);  // TCP 캡쳐 스레드 생성
                pthread_detach(capture_thd);                                                   //스레드 자원 해제
            }
        } else if (menuItem == 2 && isDigit == 1) {  // 캡쳐 중지
            if (!captureStart)
                fprintf(stdout, "시작 중이 아닙니다 !!\n");
            else {
                captureStart = false;
                fprintf(stdout, "캡쳐 중지.\n");
            }
        } else if (menuItem == 3 && isDigit == 1) {  // show Menu
            MenuBoard();
        } else {  // exception handling
            fprintf(stdout, "잘못 입력하셨습니다 !!\n\n");
        }
    }
    close(rawSocket);  // socket close
}

void buffer_flush() {
    while (getchar() != '\n')
        ;
}