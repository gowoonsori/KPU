# 컴퓨터 네트워크 과제 ( raw socket 이용하여 packet capture prgram 만들기 )

개발 환경 : Linux Ubuntu 20.04 (wsl2) / C / vscode
동작 환경 : Linux / cli

2계층인 Ethernet 정보부터 패킷을 수집하여 앞에서부터 잘라내면서, TCP/UDP/ICMP 프로토콜 캡쳐 하는 프로그램

사용방법 : Makefile로 컴파일 후 프로그램을 `su`권한으로 실행시키면 된다.
요약내용은 command에 표시되며, 상세 내용은 자동으로 .txt파일에 저장된다.
