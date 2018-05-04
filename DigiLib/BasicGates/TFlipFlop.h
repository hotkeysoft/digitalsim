#pragma once

#include "Core\GateBase.h"
#include "Core\CompositeGate.h"

#ifdef  DIGILIB_EXPORTS 
/*Enabled as "export" while compiling the dll project*/
#define DllExport __declspec(dllexport)  
#else
/*Enabled as "import" in the Client side for using already created dll file*/
#define DllExport __declspec(dllimport)  
#endif

namespace DigiLib {
	namespace BasicGates {

		class DllExport TFlipFlop : public Core::CompositeGate
		{
		public:
			static Core::GatePtr Create();

			Core::GatePtr Clone(const char* name) override;

			void ComputeState() override;

		protected:
			TFlipFlop() noexcept;
			void Init() override;

			Core::GatePtr m_nor1;
			Core::GatePtr m_nor2;

			struct shared_enabler;
		};
	}
}