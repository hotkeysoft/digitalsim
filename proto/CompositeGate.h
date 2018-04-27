#pragma once

#include <set>
#include <map>
#include "IOPin.h"
#include "IOConnection.h"
#include "GateBase.h"

typedef std::map<std::string, GateBase*> GateMapType;

class CompositeGate : public GateBase
{
public:
	CompositeGate();
	CompositeGate(const char* name);
	virtual ~CompositeGate();

	virtual void SetName(const char *name);

	virtual GateBase* Clone(const char* name);

	using GateBase::AddInput;
	using GateBase::AddOutput;

	virtual void AddGate(const char* name, GateBase* gate);
	virtual size_t GetGateCount() { return m_internalGates.size(); }
	virtual GateBase* GetGate(const char*  name);
	virtual GateMapType& GetInternalGates() { return m_internalGates; }

	void ConnectPins(IOPin* source, IOPin* target);

protected:
	GateMapType m_internalGates;

	static void InternalCloneOutputs(CompositeGate* source, CompositeGate* clone);
	static void InternalCloneInputs(CompositeGate* source, CompositeGate* clone);
	static void InternalCloneGates(CompositeGate* source, CompositeGate* clone);
	static void InternalCloneLinks(GateBase* source, GateBase* clone);
	static void InternalCloneInnerLinks(GateBase* source, GateBase* clone);

	void ValidateGateName(const char* name, bool checkDuplicate = true);
};

