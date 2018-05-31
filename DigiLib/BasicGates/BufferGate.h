#pragma once

#include <vector>
#include "Core/GateBase.h"
#include "Core/IOPin.h"

namespace DigiLib {
	namespace BasicGates {

		class DllExport BufferGate : public Core::GateBase
		{
		public:
			static Core::GatePtr Create(size_t width = 1);
			Core::GatePtr Clone(const char * name, bool deep = true) override;

			void ComputeState() override;

		protected:
			BufferGate(size_t width);
			void Init() override;

			Core::IOPinPtr m_in;
			Core::IOPinPtr m_en;
			Core::IOPinPtr m_out;

			size_t m_width;

			struct shared_enabler;
		};
	}
}