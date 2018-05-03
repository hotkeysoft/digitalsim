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

		class DllExport NOTGate : public Core::GateBase
		{
		public:
			static Core::GatePtr Create();
			Core::GatePtr Clone(const char* name) override;

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