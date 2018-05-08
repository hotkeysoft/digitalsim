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