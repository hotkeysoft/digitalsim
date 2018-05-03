#pragma once
#include <memory>
#include "Common.h"
#include "IOState.h"

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
		class DllExport IOPin : public std::enable_shared_from_this<IOPin>
		{
		public:
			enum IO_DIRECTION { INPUT, OUTPUT, OUTPUT_HI_Z };

		public:
			IOPin(GateRef parentGate, size_t id, const char * name, size_t width, IO_DIRECTION direction);

			virtual ~IOPin() = default;
			IOPin(const IOPin&) = delete;
			IOPin& operator=(const IOPin&) = delete;
			IOPin(IOPin&&) = delete;
			IOPin& operator=(IOPin&&) = delete;

			virtual IOPinPtr Clone(GateRef cloneParent);

			size_t GetID() { return m_id;  }
			virtual std::string GetRawName() { return m_name; }
			virtual std::string GetName() { return m_name; }
			virtual std::string GetFullName();			
			GateRef GetParent() noexcept { return m_parentGate; }
			IO_DIRECTION GetDirection() noexcept { return m_direction; }

			virtual size_t GetWidth() { return m_width; }
			virtual IOState Get() noexcept { return m_state; };
			virtual void Set(IOState state);

			virtual void ConnectTo(IOPinPtr);

		protected:					
			virtual size_t GetOffset() { return 0; }
			size_t m_id;
			size_t m_width;
			GateRef m_parentGate;

			std::string m_name;
			IO_DIRECTION m_direction;

			IOState m_state;

			void ComputePinState();
		};
	}
}