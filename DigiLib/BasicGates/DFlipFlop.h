#pragma once

#include "Core/GateBase.h"
#include "Core/CompositeGate.h"

namespace DigiLib {
	namespace BasicGates {

		class DllExport DFlipFlop : public Core::CompositeGate
		{
		public:
			static Core::GatePtr Create(bool positiveTriggered = true);
			Core::GatePtr Clone(const char * name, bool deep = true) override;

		protected:
			DFlipFlop(bool positiveTriggered = true) noexcept;
			void Init() override;

			struct shared_enabler;

			bool m_positiveTriggered;
		};
	}
}