#pragma once

#include "Core/GateBase.h"

namespace DigiLib {
	namespace BasicGates {

		class DllExport WireGate : public Core::GateBase
		{
		public:
			static Core::GatePtr Create(size_t delay = 1);

			Core::GatePtr Clone(const char * name, bool deep = true) override;

			void ComputeState() override;

		protected:
			WireGate(size_t delay = 1) noexcept;
			void Init() override;

			Core::IOPinPtr m_in;
			Core::IOPinPtr m_out;

			struct shared_enabler;
		};
	}
}