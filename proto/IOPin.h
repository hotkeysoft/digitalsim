#pragma once

#include <set>
class GateBase;

class IOPin
{
public:
	enum IO_STATE { LOW = 0, HI = 1, UNDEF = -1};
	enum IO_DIRECTION { INPUT, OUTPUT };

	IOPin() {}
	IOPin(GateBase *parentGate, const char * name, IO_DIRECTION direction);
	virtual ~IOPin();

	std::string GetName() { return m_name; }
	IO_DIRECTION GetDirection() { return m_direction; }

	virtual IO_STATE Get() { return m_state; };
	virtual void Set(IO_STATE state);

	virtual void ConnectTo(IOPin*);

protected:
	GateBase* m_parentGate;

	std::string m_name;
	IO_DIRECTION m_direction;

	IO_STATE m_state;
	
	std::set<IOPin*> m_connectedPins;
};