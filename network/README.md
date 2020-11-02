# 컴퓨터 네트워크 과제 ( raw socket 이용하여 packet capture prgram 만들기 )

개발 환경 : Linux Ubuntu 20.04 (wsl2) / C / vscode
동작 환경 : Linux / cli

IPv4를 기반으로 2계층인 Ethernet 정보부터 패킷을 수집하여 앞에서부터 잘라내면서,Ethernet header | ip header | TCP/UDP/ICMP header | data(payload) 캡쳐 하는 프로그램

사용방법 : Makefile로 컴파일 후 프로그램을 `su`권한으로 실행시키면 된다.
요약내용은 command에 표시되며, 상세 내용은 자동으로 .txt파일에 저장된다.

start 시 protocol port ip options 순으로 입력하여 실행.
 - protocol : 
   - * (all ) : tcp / udp / icmp  프로토콜 캡쳐
   - tcp : tcp 프로토콜 캡쳐
   - udp : udp 프로토콜 캡쳐
   - icmp : icmp 프로토콜 캡쳐
 - port :
   - * (all) : 0~65535 모든 포트번호 캡쳐
   - 0~65535 입력 [ http : 80 / dns : 53 ]
 - ip : 
   - * (all ) : 0.0.0.0 ~ 255.255.255.255 모든 ip주소 캡쳐
   - 0.0.0.0 ~ 255.255.255.255 입력 [ localhost : 127.0.0.1 ]
 - options : 
   - s : 캡쳐 패킷 data 요약내용으로 출력
   - a : 캡쳐 패킷 data ascii로 출력
   - x : 캡쳐 패킷 data hex로 출력
