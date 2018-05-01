/*
 * TCPHandler_UI.cpp
 *
 *  Created on: 16.04.2018
 *      Author: HN / MG
 */

#include "TCPHandler_UI.h"
#include "Statemachine.h"
#include "inetLib.h" 
#include "taskLib.h"
#include "strLib.h" 
#include "fioLib.h"

#define SERVER_PORT_NUM         4444   /* server's port number for bind() */ 
#define SERVER_WORK_PRIORITY    100    /* priority of server's work task */ 
#define SERVER_STACK_SIZE       0x10000  /* stack size of server's work task */ 
#define SERVER_MAX_CONNECTIONS  4      /* max clients connected at a time */ 
#define REQUEST_MSG_SIZE        1024   /* max size of request message */ 
#define REPLY_MSG_SIZE          500    /* max size of reply message */ 
 
static char errorMsg[] = 	"ERROR: >> INVALID INPUT!\n";
static char replyMsg[] =	"YOU ARE NOW CONNECTED TO THE CONVEYOR BELT \n"
							"Use the following syntax to set parameters as needed:\n"
							"----------------------------------------------------\n"
		                    "mode# : Select mode with # (1 = local, 2 = chain)\n"
		                    "Possible commands in local mode: \n"
							"mD+   : To set conveyor in forward direction \n"
							"mD-   : To set conveyor in backward direction \n"
							"mS#   : To set the final speed with # [1...22] \n"
							"start : To start the profile \n"
							"slow  : To start the belt in slow motion \n"
							"stop  : To stop the belt \n";

StateMachine* pStateMachine;
STATUS tskTCPServerUI (TCPHandler_UI* pHandler);

TCPHandler_UI::TCPHandler_UI(SysControl* pSctrl, MotorControl* pMctrl){
	m_pSysControl = pSctrl;
	m_pMotorControl = pMctrl;
}

TCPHandler_UI::~TCPHandler_UI(){
	m_pSysControl = NULL;
	m_pMotorControl = NULL;
}

void TCPHandler_UI::startServer(){
	taskSpawn ("TCP_Server_UI", 122, 0,0x1000, (FUNCPTR) tskTCPServerUI,(int)this,0,0,0,0,0,0,0,0,0);
}

/****************************************************************************
 * --------------------------  Input Processing -------------------------
 ***************************************************************************/
void TCPHandler_UI::processRequest(char myBuffer[80], int sFd){
	
	if(strncmp(myBuffer, "mode1",5)==0){
		pStateMachine->sendEvent("setOpMode(OPMODE_LOCAL)");
		sprintf(myBuffer ,"CHANGE TO LOCAL MODE REQUESTED \n");
	}
	else if(strncmp(myBuffer, "mode2",5)==0){
		pStateMachine->sendEvent("setOpMode(OPMODE_CHAIN)");
		sprintf(myBuffer ,"CHANGE TO CHAIN MODE REQUESTED \n");
	}
	else if(m_pSysControl->getOpMode() == OPMODE_LOCAL){
		if(myBuffer[0]=='m' && myBuffer[1]=='D'){
			// set motor direction forward
			if(myBuffer[2]=='+'){
				if(m_pMotorControl->getMotorState() == MOTOR_STOP){
					m_pMotorControl->setDirection(true);
					sprintf(myBuffer ,"BELT DIRECTION IS SET TO FORWARD! \n");
				}
				else{
					sprintf(myBuffer ,"MOTOR IS BUSY \n");
				}
			}
			// set motor direction backwards
			else if(myBuffer[2]=='-'){
				if(m_pMotorControl->getMotorState() == MOTOR_STOP){
					m_pMotorControl->setDirection(false);
					sprintf(myBuffer ,"BELT DIRECTION IS SET TO BACKWARD!\n");
				}
				else{
					sprintf(myBuffer ,"MOTOR IS BUSY \n");
				}
			}
			// errorMsg if no valid input was entered
			else{
				sprintf(myBuffer ,errorMsg);
			}
		}
		// Set motor speed in local mode
		else if(myBuffer[0]=='m' && myBuffer[1]=='S'){
			// extract character Nr 3-5
			char intBuffer[3];
			memcpy(intBuffer, myBuffer + 2 /* Offset */, 3/* Length */);
			int desiredSpeed;
			desiredSpeed = atoi(intBuffer) * 100;

			if(desiredSpeed <= 2200 && desiredSpeed >= 100){
				if(m_pMotorControl->getMotorState() == MOTOR_STOP){
					m_pMotorControl->setMotorSpeedFinal(desiredSpeed);
					sprintf(myBuffer ,"MOTOR SPEED IS SET TO: %d RPM\n", desiredSpeed);
				}
				else{
					sprintf(myBuffer ,"MOTOR IS BUSY \n");
				}
			}
			// errorMsg if no valid input was entered
			else{
				sprintf(myBuffer ,errorMsg);
			}
		}
		// start motor
		else if(strncmp(myBuffer, "start",5)==0){
			if(m_pMotorControl->getMotorState() == MOTOR_STOP){
				pStateMachine->sendEvent("startRamp()");
				sprintf(myBuffer ,"MOTOR STARTED!\n");
			}
			else{
				sprintf(myBuffer ,"MOTOR IS BUSY \n");
			}
		}
		// start stop
		else if(strncmp(myBuffer, "stop",4)==0){
			if(m_pMotorControl->getMotorState() == MOTOR_SLOW){
				pStateMachine->sendEvent("stop()");
				sprintf(myBuffer ,"MOTOR STOPED!\n");
			}
			else{
				sprintf(myBuffer ,"MOTOR IS NOT MOVING SLOW \n");
			}
		}
		// start motor slow
		else if(strncmp(myBuffer, "slow",4)==0){
			if(m_pMotorControl->getMotorState() == MOTOR_STOP){
				pStateMachine->sendEvent("moveSlow()");
				sprintf(myBuffer ,"MOTOR STARTED ON SLOW!\n");
			}
			else{
				sprintf(myBuffer ,"MOTOR IS BUSY \n");
			}
		}
		// errorMsg if no valid input was entered
		else{
			sprintf(myBuffer ,errorMsg);
		}
	}
	
	printf(myBuffer);							/* print to console    */
	write (sFd, myBuffer, strlen (myBuffer));	/* print to TCP-Client */
}

/*************************************************************************
------------------------- Setting up the Server Tasks --------------------
SOURCE: http://www.vxdev.com/docs/vx55man/vxworks/netguide/c-sockets.html
***************************************************************************/

/****************************************************************************
 * --------------------------  TCP-WORKER TASK  -------------------------
 ***************************************************************************/
void tcpServerWorkTask(int sFd, char* address, u_short port, TCPHandler_UI* pHandler){ // server's socket fd, client's socket address,  client's socket port
	char myBuffer[80];
	char String[80];
	float floatWert;
    int	nRead;     // number of bytes read
     
    write (sFd, replyMsg, sizeof (replyMsg));	// send reply message to client
    
    while ( (nRead = fioRdString (sFd, myBuffer, sizeof (myBuffer))) > 0){
    	// Ausgabe in der Konsole
        printf ("MESSAGE FROM CLIENT (Internet Address %s, port %d):\n%s\n", address, port, myBuffer); 
        /************ porcessing of received Input ************************/
        pHandler->processRequest(myBuffer, sFd);
        /******************************************************************/
    } 
 
    if (nRead == ERROR){
    	perror ("read"); /* error from read() */ 
    }

    close (sFd);						/* close server socket connection */    
}

/****************************************************************************
 * ------------------------------  TCP-Server  ------------------------------
 ***************************************************************************/

STATUS tskTCPServerUI (TCPHandler_UI* pHandler) 
	{ 
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
		sprintf (workName, "tskTcpWork%d", ix++); 
		if (taskSpawn(workName, SERVER_WORK_PRIORITY, 0, SERVER_STACK_SIZE,
				(FUNCPTR) tcpServerWorkTask, newFd,
				(int) inet_ntoa (clientAddr.sin_addr), ntohs (clientAddr.sin_port),
				(int) pHandler, 0, 0, 0, 0, 0, 0) == ERROR) 
			{ 
			perror ("taskSpawn"); 
			close (newFd); 
			} 
		} 
	} 
