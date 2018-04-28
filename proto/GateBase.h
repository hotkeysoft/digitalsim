#pragma once

#include <set>
#include <map>
#include "IOPin.h"
#include "IOConnection.h"

typedef std::map<std::string, IOPin*> IOPinMapType;
typedef std::set<IOConnection> PinConnectionsType;
typedef std::map<IOPin*, PinConnectionsType> ConnectedPinsType;
typedef std::map<IOPin::IO_DIRECTION, std::map<IOPin::IO_DIRECTION, bool> > AllowedConnectionMapType;

class GateBase
{
public:
	GateBase();
	GateBase(const char* name);
	virtual ~GateBase();

	std::string GetName() { return m_name; }
	std::string GetFullName();
	virtual void SetName(const char*);

	virtual GateBase* Clone(const char* name) = 0;

	virtual IOPin* GetPin(const char*  name);

	virtual PinConnectionsType& GetConnectedPins(const char* source);
	virtual PinConnectionsType& GetConnectedPins(IOPin* source);
	virtual ConnectedPinsType& GetConnectedPins() { return m_connectedPins; }

	GateBase* GetParent() { return m_parent; }
	virtual void SetParent(GateBase* parent);

	virtual void ComputeState() {};

	virtual void Clock() {};

	size_t GetInputCount() { return m_inputPins.size(); }
	size_t GetOutputCount() { return m_outputPins.size(); }

	IOPinMapType GetInputPins() { return m_inputPins;  }
	IOPinMapType GetOutputPins() { return m_outputPins; }

	void ConnectPins(IOPin* source, IOPin* target);

protected:
	virtual IOPin* AddInput(const char*  name, int8_t width = 1);
	virtual IOPin* AddOutput(const char*  name, int8_t width = 1, IOPin::IO_DIRECTION dir = IOPin::IO_DIRECTION::OUTPUT);

	std::string m_name;
	GateBase* m_parent;

	ConnectedPinsType m_connectedPins;
	
	IOPinMapType m_inputPins;
	IOPinMapType m_outputPins;

	static AllowedConnectionMapType m_insideInsideMap;
	static AllowedConnectionMapType m_insideParentMap;
	static AllowedConnectionMapType m_parentInsideMap;
	static void InitAllowedConnectionMaps();

	bool IsValidPinName(const char* name);
	void ValidatePinName(const char* name);
	void ValidatePinWidth(int8_t width);

	bool IsValidGateName(const char* name);
	void ValidateGateName(const char* name);
};

