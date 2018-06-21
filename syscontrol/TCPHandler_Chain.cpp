/*
 * TCPHandler_Chain.cpp
 *
 *  Created on: 16.04.2018
 *      Author: HN / MG
 */

#include "TCPHandler_Chain.h"
#include "Statemachine.h"
#include <string>
#include "inetLib.h" 
#include "taskLib.h"
#include "strLib.h" 
#include "fioLib.h"

#define SERVER_PORT_NUM         5555   /* server's port number for bind() */ 
#define SERVER_WORK_PRIORITY    100    /* priority of server's work task */ 
#define SERVER_STACK_SIZE       0x10000  /* stack size of server's work task */ 
#define SERVER_MAX_CONNECTIONS  4      /* max clients connected at a time */ 
#define REQUEST_MSG_SIZE        1024   /* max size of request message */ 
#define REPLY_MSG_SIZE          500    /* max size of reply message */ 

static char errorMsg[] = 	"ERROR: >> INVALID INPUT!\n";
static char replyMsg[] =	"YOU ARE CONNECTED TO CHAIN-MODE SERVER\n";

STATUS tskTCPServerChain (TCPHandler_Chain* pHandler);
STATUS tskTCPClientChain(TCPHandler_Chain* pHandler);

TCPHandler_Chain::TCPHandler_Chain(SysControl* pSctrl, string addrMaster){
	m_pSysControl = pSctrl;
	m_AddrMaster = addrMaster;
	m_AddrRCB = "";
	m_AddrLCB = "";
	m_AddrRCBset = false;
}

TCPHandler_Chain::~TCPHandler_Chain(){
	m_pSysControl = NULL;
}

void TCPHandler_Chain::startServer(){
	taskSpawn ("TCP_Server_Chain", 123, 0,0x1000, (FUNCPTR) tskTCPServerChain,(int)this,0,0,0,0,0,0,0,0,0);
}

void TCPHandler_Chain::startClient(){
	taskSpawn ("TCP_Client_Chain", 124, 0,0x1000, (FUNCPTR) tskTCPClientChain,(int)this,0,0,0,0,0,0,0,0,0);
}

bool TCPHandler_Chain::getAddrRCBset(){
	return m_AddrRCBset;
}

string TCPHandler_Chain::getAddrMaster(){
	return m_AddrMaster;
}

string TCPHandler_Chain::getAddrRCB(){
	return m_AddrRCB;
}

void TCPHandler_Chain::sendToRCB(Command cmd){
	switch(cmd){
		case READY:		write(m_SocketRCB, "Ready\r\n", 7);
						printf("MESSAGE TO RCB: Ready\n");
						break;
		case REQUEST:	write(m_SocketRCB, "Request\r\n", 9);
						printf("MESSAGE TO RCB: Request\n");
						break;
		case WAIT:		write(m_SocketRCB, "Wait\r\n", 6);
						printf("MESSAGE TO RCB: Wait\n");
						break;
		case RELEASE:	write(m_SocketRCB, "Release\r\n", 9);
						printf("MESSAGE TO RCB: Release\n");
						break;
		default:		return;
	}
}

void TCPHandler_Chain::sendToLCB(Command cmd){
	switch(cmd){
		case READY:		write(m_SocketLCB, "Ready\r\n", 7);
						printf("MESSAGE TO LCB: Ready\n");
						break;
		case REQUEST:	write(m_SocketLCB, "Request\r\n", 9);
						printf("MESSAGE TO RCB: Request\n");
						break;
		case WAIT:		write(m_SocketLCB, "Wait\r\n", 6);
						printf("MESSAGE TO RCB: Wait\n");
						break;
		case RELEASE:	write(m_SocketLCB, "Release\r\n", 9);
						printf("MESSAGE TO LCB: Release\n");
						break;
		default:		return;	
	}
}

void TCPHandler_Chain::setSocketRCB(int sFd){
	m_SocketRCB = sFd;
}

void TCPHandler_Chain::setSocketLCB(int sFd){
	m_SocketLCB = sFd;
}

int TCPHandler_Chain::getSocketRCB(){
	return m_SocketRCB;
}

int TCPHandler_Chain::getSocketLCB(){
	return m_SocketLCB;
}

/****************************************************************************
 * --------------------------  Input Processing -------------------------
 ***************************************************************************/
void TCPHandler_Chain::processClientRequest(char myBuffer[80], int sFd){
	if(strncmp(myBuffer, "Request",7)==0){
		m_pSysControl->getStateMachine()->sendEvent("setCommand(REQUEST)");
	}
	else if(strncmp(myBuffer, "Ready",5)==0){
		m_pSysControl->getStateMachine()->sendEvent("setCommand(READY)");
	}
	else if(strncmp(myBuffer, "Release",7)==0){
		m_pSysControl->getStateMachine()->sendEvent("setCommand(RELEASE)");
	}
	else if(strncmp(myBuffer, "Wait",4)==0){
		m_pSysControl->getStateMachine()->sendEvent("setCommand(WAIT)");
	}
	else if(strncmp(myBuffer, "Right ",6)==0){
		m_AddrRCB = string(myBuffer+6, 8);
		m_AddrRCBset = true;
		startClient();
		m_pSysControl->getStateMachine()->sendEvent("setOpMode(OPMODE_CHAIN)");
	}
	else{
	} 
}

/****************************************************************************
 * --------------------------  TCP-WORKER TASK  -------------------------
 * For connections with Master and LCB
 ***************************************************************************/
void tcpServerChainWorkTask(int sFd, char* address, u_short port, TCPHandler_Chain* pHandler){ // server's socket fd,  client's socket address, client's socket port
	char myBuffer[80];
    int nRead;          /* number of bytes read */ 
        
	while ( (nRead = fioRdString (sFd, myBuffer, sizeof (myBuffer))) > 0 ){
		printf ("MESSAGE FROM CLIENT (Internet Address %s, port %d): %s\n", address, port, myBuffer); 
		/************ porcessing of received Input ************************/
		pHandler->processClientRequest(myBuffer, sFd);
		pHandler->setSocketLCB(sFd);
		/******************************************************************/
    }

    if (nRead == ERROR)                 /* error from read() */ 
        perror ("read"); 
 
    close (sFd);						/* close server socket connection */    
}

/****************************************************************************
 * ------------------------------  TCP-Server  ------------------------------
 * For connections to Master and LCB
 ***************************************************************************/
STATUS tskTCPServerChain(TCPHandler_Chain* pHandler){ 
	struct sockaddr_in  serverAddr;    /* server's socket address */ 
	struct sockaddr_in  clientAddr;    /* client's socket address */ 
	int                 sockAddrSize;  /* size of socket address structure */ 
	int                 sFd;           /* socket file descriptor */ 
	int                 newFd;         /* socket descriptor from accept */ 
	int                 ix = 0;        /* counter for work task names */ 
	char                workName[16];  /* name of work task */
	
	/* set up the local address */
	sockAddrSize = sizeof (struct sockaddr_in); 
	bzero ((char *) &serverAddr, sockAddrSize); 
	serverAddr.sin_family = AF_INET; 
	serverAddr.sin_len = (u_char) sockAddrSize; 
	serverAddr.sin_port = htons (SERVER_PORT_NUM); 
	serverAddr.sin_addr.s_addr = htonl (INADDR_ANY);
	 /* create a TCP-based socket */
	 if ((sFd = socket (AF_INET, SOCK_STREAM, 0)) == ERROR) 
		{ 
		perror ("socket"); 
		return (ERROR); 
		}
	 /* bind socket to local address */
	 if (bind (sFd, (struct sockaddr *) &serverAddr, sockAddrSize) == ERROR) 
		{ 
		perror ("bind"); 
		close (sFd); 
		return (ERROR); 
		}
	 /* create queue for client connection requests */
	 if (listen (sFd, SERVER_MAX_CONNECTIONS) == ERROR) 
		{ 
		perror ("listen"); 
		close (sFd); 
		return (ERROR); 
		}
	 /// New incomming Connections will result in crating a new Task
	 while(1)
		{ 
		 socklen_t len = sizeof(clientAddr);
		if ((newFd = accept (sFd, (struct sockaddr *) &clientAddr,
				&len)) == ERROR) 
			{ 
			perror ("accept"); 
			close (sFd); 
			return (ERROR); 
			}
		sprintf (workName, "tskTcpChainWork%d", ix++); 
		if (taskSpawn(workName, SERVER_WORK_PRIORITY, 0, SERVER_STACK_SIZE,
				(FUNCPTR) tcpServerChainWorkTask, newFd,
				(int) inet_ntoa (clientAddr.sin_addr), ntohs (clientAddr.sin_port),
				(int) pHandler, 0, 0, 0, 0, 0, 0) == ERROR) 
			{ 
			perror ("taskSpawn"); 
			close (newFd); 
			} 
		} 
} 

/****************************************************************************
 * ------------------------------  TCP-Client  ------------------------------
 * For connection to RCB
 ***************************************************************************/
STATUS tskTCPClientChain(TCPHandler_Chain* pHandler){
	//struct request      myRequest;     /* request to send to server */
	struct sockaddr_in  serverAddr;    /* server's socket address */
	char                replyBuf[REPLY_MSG_SIZE]; /* buffer for reply */
	char                reply;         /* if TRUE, expect reply back */
	int                 sockAddrSize;  /* size of socket address structure */
	int                 sFd;           /* socket file descriptor */
	int                 nRead;          /* length of message */
	char myBuffer[80];
	
	/* create client's socket */
	if ((sFd = socket (AF_INET, SOCK_STREAM, 0)) == ERROR){
		perror ("socket");
		return (ERROR);
	}
	
	/* bind not required - port number is dynamic */
	/* build server socket address */
	sockAddrSize = sizeof (struct sockaddr_in);
	bzero ((char *) &serverAddr, sockAddrSize);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_len = (u_char) sockAddrSize;
	serverAddr.sin_port = htons (SERVER_PORT_NUM);
	
	if ( (serverAddr.sin_addr.s_addr = inet_addr(pHandler->getAddrRCB().c_str())) == ERROR){
		perror ("unknown server name");
		close (sFd);
		return (ERROR);
	}
	
	/* connect to server */
	if (connect (sFd, (struct sockaddr *) &serverAddr, sockAddrSize) == ERROR){
		perror ("connect");
		close (sFd);
		return (ERROR);
	}
	
	pHandler->setSocketRCB(sFd);
	
	while(true){
		if( (nRead = fioRdString (sFd, myBuffer, sizeof (myBuffer))) > 0 ){
			// Ausgabe in der Konsole
			printf ("MESSAGE FROM RCB:\n%s\n", myBuffer); 
			/************ porcessing of received Input ************************/
			pHandler->processClientRequest(myBuffer, sFd);
			/******************************************************************/
		}
	}
	
	close (sFd);
	return (OK);
}
