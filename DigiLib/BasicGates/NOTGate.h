#pragma once

#include "Core/GateBase.h"

namespace DigiLib {
	namespace BasicGates {

		class DllExport NOTGate : public Core::GateBase
		{
		public:
			static Core::GatePtr Create();
			Core::GatePtr Clone(const char * name, bool deep = true) override;

			void ComputeState() override;

		protected:
			NOTGate() noexcept;
			void Init() override;

			Core::IOPinPtr m_in;
			Core::IOPinPtr m_out;

			struct shared_enabler;
		};
	}
}