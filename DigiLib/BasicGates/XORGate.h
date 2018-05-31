#pragma once

#include "Core/GateBase.h"

namespace DigiLib {
	namespace BasicGates {

		class DllExport XORGate : public Core::GateBase
		{
		public:
			static Core::GatePtr Create();
			Core::GatePtr Clone(const char * name, bool deep = true) override;

			void ComputeState() override;

		protected:
			XORGate() noexcept;
			void Init() override;

			Core::IOPinPtr m_in1;
			Core::IOPinPtr m_in2;
			Core::IOPinPtr m_out;

			struct shared_enabler;
		};
	}
}