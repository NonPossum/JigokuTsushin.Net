#pragma comment(lib,"ws2_32.lib")
#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>
#include <iostream>
#include <map>
#include <thread>
#include <vector>
#include <fstream>
#include <ctime>
////////////////////////////////
#define PORT 80
#define HOST "127.0.0.1"

void erro(const char *str,UINT exitcode);

int init(std::vector<char> buff,char (&verb)[5],char (&pth)[20]);

void parse(char (&type)[20],char (&pth)[20]);

std::vector<char> loadfile(char (&pth)[20]);

void sendata(SOCKET n_socket,char (&status_code)[4],char (&mtype)[50],std::vector<char> dataa);

int get(SOCKET n_socket,char (&pth)[20]);

int post(SOCKET n_socket,char (&recvbuf)[0x1024]);

int connection(SOCKET n_socket);


// formats: .html .gif .ico .css .js
// respond on get and push
// only at 24pm

//g++ server.cc -o serwer.exe -lws2_32 -std=c++11


int main(void)
{

	WORD wVersionRequested;

	WSADATA wsaData;

	int wsaerr;

	wVersionRequested = MAKEWORD(2, 2);

	wsaerr = WSAStartup(wVersionRequested, &wsaData);

	if (wsaerr != 0){erro("Server: The Winsock dll not found!\n",3);};

	SOCKET m_socket;

	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	 
	if (m_socket == INVALID_SOCKET){ WSACleanup();erro("Socket Error",3);};

	SOCKADDR_IN servsock;
	servsock.sin_family = AF_INET;
	servsock.sin_addr.s_addr = inet_addr(HOST);
	servsock.sin_port = htons(PORT);

	
	if (bind(m_socket, (SOCKADDR*)&servsock, sizeof(servsock)) == SOCKET_ERROR)
	{	closesocket(m_socket);
		WSACleanup();
		erro("Bind Error",3);
	}
	 
	if (listen(m_socket, 1) == SOCKET_ERROR)
	{	closesocket(m_socket);
		WSACleanup();
		erro("Listen Error",3);
	}
	
	puts("Server: Waiting for a client to connections...\n");



	while(true)
	{
		time_t tt;
    	time(&tt);
    	tm TM = *localtime( &tt );
		DWORD thr;


		if (TM.tm_hour != 24)
		{

			SOCKET AcceptSocket = accept(m_socket, NULL,NULL);
		    if (AcceptSocket == INVALID_SOCKET) {
		        wprintf(L"accept failed with error: %ld\n", WSAGetLastError());
		        closesocket(m_socket);
		        WSACleanup();
		        return 1;
		    } else wprintf(L"Client connected.\n");
			CreateThread(NULL, 0,(LPTHREAD_START_ROUTINE)connection,
			(LPVOID)AcceptSocket, 0, &thr);

		}



	}

	return 0;
}


int connection(SOCKET n_socket){

	const int  recvbuflen = 0x1024;

	char recvbuf[recvbuflen];
	bool req = false;
	const char GET[5]  = "GET ";
	const char POST[5] = "POST";
	
	recv(n_socket,recvbuf,recvbuflen,0);

	int i = 0;
	std::vector<char> finalbuf;
	for (i; i < recvbuflen; ++i)
	{
		if(recvbuf[i] == '\r' && recvbuf[i + 3] == '\n'){req = true;break;};
		finalbuf.push_back(recvbuf[i]);
	}
	if(!req){
	   closesocket(n_socket);
	   return 3;
	   ExitThread(0);
	}

 	char verb[5] = "";
 	char pth[20] = "";


 	if(init(finalbuf,verb,pth) == 3){
	   closesocket(n_socket);
	   return 3;
	   ExitThread(0);
	}


	if (strcmp(verb,GET) == 0)
	{
		get(n_socket,pth);

	}
	else
	{
		verb[4] = '\0';
		
		if (strcmp(verb,POST) == 0){
			post(n_socket,recvbuf);
		}
		else{
			closesocket(n_socket);
			return 3;
			ExitThread(0);
		}


	}

	return 0;
}


void erro(const char *str,UINT exitcode){
	printf("%s\n",str);
	ExitProcess(exitcode);
	ExitThread(0);
}

int init(std::vector<char> buff,char (&verb)[5],char (&pth)[20]){

	memset(verb,0,5);
 	bool flag = true;
	int itr = 0;
	
 	for(auto x: buff){ 
 		if (flag)
 		{
			if(itr > sizeof(verb)) return 3;
 			verb[itr] = x;
 		}
 		else{
 			if(itr > sizeof(pth)) return 3;
 			pth[itr] = x;
 			
 		}
 		if(x == ' ')
 		{
 			if (flag == false)
 			{
 				break;
 			}
 			flag = false;
 			itr = 0;
 			continue;
 		}	
		itr++;
 	}
	return 0;
}

void parse(char (&type)[20],char (&pth)[20]){
	int x = 1;
	int i;

	if(pth[0] == '/' && pth[1] == ' '){

		strcpy(pth,"index.html");
		strcpy(type,".html"); 
		return;

	}
	else{

	for (i = 0; pth[i] != '\0'; ++i) {
        	pth[i] = pth[x];
			x = x + 1;
   	 }

	x = 0;
	for (i = 0; pth[i] != '\0'; ++i) {
		if (pth[i] == '.')
		{
			for (size_t y = i; pth[y] != '\0'; y++)
			{
				type[x] = pth[y];
				x = x + 1;
			}
		}
   	 }

	}

}

int post(SOCKET n_socket,char (&recvbuf)[0x1024]){

	char buff[20] =  " ";

	for (int i = 0; i < sizeof(recvbuf); ++i)
	{

		if (recvbuf[i] == '\n' && recvbuf[i+1] == 'Y') 
		{
			for (int z = 0,y = i; z < 20; ++z)
			{
				buff[z] = recvbuf[y++];
			}
		}


	}

	srand(time(NULL));
	int r  = rand();
	std::string name = std::to_string(r);

	std::ofstream revng(name);
  	revng << buff;

  	revng.close();

	send(n_socket,"HTTP 201: Created", 18, 0);


 	closesocket(n_socket);
	ExitThread(0);
 	return 0;

}
int get(SOCKET n_socket,char (&pth)[20]){


std::map<char*, char*> mime = {
    { ".ico","image/vnd.microsoft.icon"},
    { ".gif", "image/vnd.sealedmedia.softseal.gif" },
    { ".js", "text/javascript"},
    { ".css","text/css"},
    { ".html", "text/html;charset=utf-8"}
				};

	char mtype[50] = " ";
	char type[20] = "";
	char status_code[4] = "200";
	
	parse(type,pth);
	
	std::vector<char> dataa = loadfile(pth);


	//Check mime type
	for (auto& x: mime) {
		if (type[1] == x.first[1])
		{
			strcpy(mtype,x.second);
			break;
		}
		else{
			strcpy(mtype,"application/binary");
		}
	    
	  }

  	sendata(n_socket,status_code,mtype,dataa);
  	return 0;

} 

std::vector<char> loadfile(char (&pth)[20]){

    std::ifstream input(pth, std::ios::binary);

    std::vector<char> data(
         (std::istreambuf_iterator<char>(input)),
         (std::istreambuf_iterator<char>()));

    input.close();

    return data;

}

void sendata(SOCKET n_socket,char (&status_code)[4],char (&mtype)[50],std::vector<char> dataa){

	char request[300] = " ";
	std::string s = std::to_string(dataa.size());

	strncat(request, "HTTP/1.1 ",300 -strlen(request) -1);
	strncat(request, "200 OK", 300 - strlen(request) - 1);
	strncat(request, "\r\n", 300 - strlen(request) - 1);
	strncat(request, "Content-Type: ", 300 - strlen(request) - 1);
	strncat(request, mtype, 300 - strlen(request) - 1);
	strncat(request, "\r\n", 300 - strlen(request) - 1);
	strncat(request, "Content-Length: ", 300 - strlen(request) - 1);
	strncat(request, (const char*)s.c_str(), 300 - strlen(request) - 1);	
	strncat(request, "\r\n", 300 - strlen(request) - 1);
	strncat(request, "\r\n", 300 - strlen(request) - 1);

	send(n_socket,request, (int)strlen(request), 0);


	send(n_socket, (const char*)dataa.data(),  dataa.size(), 0);


	closesocket(n_socket);
	ExitThread(0);

}

