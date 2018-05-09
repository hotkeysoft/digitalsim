#pragma once
#include <memory>
#define MAX_PINS 16

namespace DigiLib
{
	namespace Core
	{
		class GateBase;
		typedef std::shared_ptr<GateBase> GatePtr;
		typedef GateBase* GateRef;

		class CompositeGate;
		typedef std::shared_ptr<CompositeGate> CompositeGatePtr;
		typedef CompositeGate* CompositeGateRef;

		class IOPin;
		typedef std::shared_ptr<IOPin> IOPinPtr;
		typedef IOPin* IOPinRef;

		class Simulator;
		typedef std::shared_ptr<Simulator> SimulatorPtr;
		typedef Simulator* SimulatorRef;
	}

	namespace Parser
	{
		class PartsBin;
		typedef std::shared_ptr<PartsBin> PartsBinPtr;
	}
}