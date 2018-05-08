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