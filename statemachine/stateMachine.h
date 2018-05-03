

#ifndef STATEMACHINE_H_
#define STATEMACHINE_H_

#include <queue>
#include <semLib.h>

#include "diaTimer.h"
#include "stateTable.h"

class SysControl;

class StateMachine {
public:
	StateMachine(SysControl* pSysControl);
	~StateMachine();
	void sendEvent(std::string myEvent);
	void runToCompletion();
	std::string myEvent;
	DiaTimer * diaTimerTable[MAXDIA];

private:
	SEM_ID volatile semQueue, semEvent;
	void putEvent(std::string myEvent);
	std::string getEvent();
	std::queue<std::string> queue;
	SysControl* m_pSysControl;
};

#endif // STATEMACHINE_H_
