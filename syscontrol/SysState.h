/*
 * SysState.h
 *
 *  Created on: 16.04.2018
 *      Author: HN / MG
 */

#ifndef SYSSTATE_H_
#define SYSSTATE_H_

enum SysState{
	STATE_IDLE,
	STATE_RECEIVEPACKET,
	STATE_TRANSFERPACKET,
	STATE_WAITFORRCB,
	STATE_DELIVERPACKET
};

#endif /* SYSSTATE_H_ */