#pragma once

#include <vector>
#include "Core/GateBase.h"
#include "Core/IOPin.h"

namespace DigiLib {
	namespace BasicGates {

		class DllExport ORGate : public Core::GateBase
		{
		public:
			static Core::GatePtr Create(size_t inputs = 2);
			Core::GatePtr Clone(const char * name, bool deep = true) override;

			void ComputeState() override;

		protected:
			ORGate(size_t pins = 2) noexcept;
			void Init() override;

			size_t m_inputs;
			std::vector<Core::IOPinPtr> m_inputPins;
			Core::IOPinPtr m_out;

			struct shared_enabler;
		};

	}
}