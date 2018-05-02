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
			BufferGate(size_t width = 1);

			GateBase* Clone(const char* name) override;

			void ComputeState() override;

		protected:
			Core::IOPinPtr m_in;
			Core::IOPinPtr m_en;
			Core::IOPinPtr m_out;

			size_t m_width;
		};
	}
}