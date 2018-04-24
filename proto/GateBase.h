#pragma once

#include <map>
#include "IOPin.h"

typedef std::map<std::string, IOPin*> IOPinMapType;

class GateBase
{
public:
	GateBase();
	virtual ~GateBase();

	void AddInput(std::string name, int8_t width = 1);
	void AddOutput(std::string name, int8_t width = 1);

	IOPin* GetPin(std::string name);

	virtual void ComputeState() = 0;

	virtual void Clock() {};

protected:
	IOPinMapType m_inputPins;
	IOPinMapType m_outputPins;
};

