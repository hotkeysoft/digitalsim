#pragma once

#include <set>
#include <map>
#include "Common.h"
#include "IOPin.h"
#include "IOConnection.h"
#include "GateBase.h"

#ifdef  DIGILIB_EXPORTS 
/*Enabled as "export" while compiling the dll project*/
#define DllExport __declspec(dllexport)  
#else
/*Enabled as "import" in the Client side for using already created dll file*/
#define DllExport __declspec(dllimport)  
#endif

namespace DigiLib
{
	namespace Core
	{
		typedef std::map<std::string, GatePtr > GateMapType;

		class DllExport CompositeGate : public GateBase
		{
		public:
			CompositeGate(const char* name);
			void SetName(const char *name) override;

			GatePtr Clone(const char* name) override;

			using GateBase::AddInput;
			using GateBase::AddOutput;

			virtual void AddGate(const char* name, GatePtr gate);
			virtual size_t GetGateCount() noexcept { return m_internalGates.size(); }
			virtual GatePtr GetGate(const char*  name);
			virtual GateMapType& GetInternalGates() noexcept { return m_internalGates; }

		protected:
			GateMapType m_internalGates;

			CompositeGatePtr thisCompositeGate() { return std::dynamic_pointer_cast<CompositeGate>(shared_from_this()); }

			static void InternalCloneOutputs(CompositeGatePtr source, CompositeGatePtr clone);
			static void InternalCloneInputs(CompositeGatePtr source, CompositeGatePtr clone);
			static void InternalCloneGates(CompositeGatePtr source, CompositeGatePtr clone);
			static void InternalCloneLinks(GatePtr source, GatePtr clone);
			static void InternalCloneInnerLinks(GatePtr source, GatePtr clone);

			void ValidateGateName(const char* name, bool checkDuplicate = true);
		};
	}
}
