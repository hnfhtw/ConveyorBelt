
#include "stateTable.h"

TableEntry :: TableEntry (
	std::string _actState,
	std::string _nextState,
	std::string _myEvent,
	int _eventTime,
	void (*_action)(SysControl* pSysControl),
	bool (*_condition)(SysControl* pSysControl)
) {
	actState = _actState;
	nextState = _nextState;
	actState = _actState;
	myEvent = _myEvent;
	eventTime = _eventTime;
	action = _action;
	condition = _condition;
}
