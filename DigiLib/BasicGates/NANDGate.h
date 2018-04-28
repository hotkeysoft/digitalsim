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

		class DllExport NANDGate : public Core::GateBase
		{
		public:
			NANDGate(size_t inputs = 2) noexcept;

			GateBase* Clone(const char* name) override;

			void ComputeState() override;
		};
	}
}
