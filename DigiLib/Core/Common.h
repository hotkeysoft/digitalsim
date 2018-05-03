#pragma once
#include <memory>
#define MAX_PINS 16

namespace DigiLib
{
	namespace Core
	{
		class GateBase;
		typedef std::shared_ptr<GateBase> GatePtr;

		class CompositeGate;
		typedef std::shared_ptr<CompositeGate> CompositeGatePtr;

		class IOPin;
		typedef std::shared_ptr<IOPin> IOPinPtr;

	}
}