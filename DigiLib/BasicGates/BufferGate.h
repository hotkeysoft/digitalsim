#pragma once

#include <vector>
#include "Core\GateBase.h"
#include "Core\IOPin.h"

#ifdef  DIGILIB_EXPORTS 
/*Enabled as "export" while compiling the dll project*/
#define DllExport __declspec(dllexport)  
#else
/*Enabled as "import" in the Client side for using already created dll file*/
#define DllExport __declspec(dllimport)  
#endif

namespace DigiLib {
	namespace BasicGates {

		class DllExport BufferGate : public Core::GateBase
		{
		public:
			static Core::GatePtr Create(size_t width = 1);
			Core::GatePtr Clone(const char* name) override;

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