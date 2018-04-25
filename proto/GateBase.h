#pragma once

#include <map>
#include "IOPin.h"

typedef std::map<std::string, GateBase*> GateMapType;
typedef std::map<std::string, IOPin*> IOPinMapType;

class GateBase
{
public:
	GateBase();
	GateBase(const char* name) : m_name(name), m_parent(NULL) {}
	virtual ~GateBase();

	virtual void AddInput(const char*  name, int8_t width = 1);
	virtual void AddOutput(const char*  name, int8_t width = 1);
	virtual void AddGate(const char* name, GateBase * gate);
	
	virtual GateBase* GetGate(const char*  name);
	virtual IOPin* GetPin(const char*  name);

	GateBase* GetParent() { return m_parent; }
	virtual void SetParent(GateBase* parent);

	virtual void ComputeState() {};

	virtual void Clock() {};

	size_t GetGateCount() { return m_internalGates.size(); }
	size_t GetInputCount() { return m_inputPins.size(); }
	size_t GetOutputCount() { return m_outputPins.size(); }

protected:
	std::string m_name;
	GateBase* m_parent;

	GateMapType m_internalGates;

	IOPinMapType m_inputPins;
	IOPinMapType m_outputPins;

	bool IsValidPinName(const char* name);
	void ValidatePinName(const char* name);
	void ValidatePinWidth(int8_t width);

	bool IsValidGateName(const char* name);
	void ValidateGateName(const char* name);
};

