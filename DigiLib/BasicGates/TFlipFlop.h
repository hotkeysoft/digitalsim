#pragma once

#include "Core/GateBase.h"
#include "Core/CompositeGate.h"

namespace DigiLib {
	namespace BasicGates {

		class DllExport TFlipFlop : public Core::CompositeGate
		{
		public:
			static Core::GatePtr Create();
			Core::GatePtr Clone(const char * name, bool deep = true) override;

		protected:
			TFlipFlop() noexcept;
			void Init() override;

			struct shared_enabler;
		};
	}
}