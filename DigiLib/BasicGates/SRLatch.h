#pragma once

#include "Core/GateBase.h"
#include "Core/CompositeGate.h"

namespace DigiLib {
	namespace BasicGates {

		class DllExport SRLatch : public Core::CompositeGate
		{
		public:
			static Core::GatePtr Create();
			Core::GatePtr Clone(const char * name, bool deep = true) override;

		protected:
			SRLatch() noexcept;
			void Init() override;
			void InitializeState() override;

			struct shared_enabler;
		};
	}
}