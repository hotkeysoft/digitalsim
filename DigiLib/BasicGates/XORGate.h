#pragma once

#include "Core\GateBase.h"

#ifdef  DIGILIB_EXPORTS 
/*Enabled as "export" while compiling the dll project*/
#define DllExport __declspec(dllexport)  
#else
/*Enabled as "import" in the Client side for using already created dll file*/
#define DllExport __declspec(dllimport)  
#endif

namespace DigiLib {
	namespace BasicGates {

		class DllExport XORGate : public Core::GateBase
		{
		public:
			XORGate() noexcept;

			GateBase* Clone(const char* name) override;

			void ComputeState() override;

		protected:
			Core::IOPin * m_in1;
			Core::IOPin * m_in2;
			Core::IOPin * m_out;
		};
	}
}