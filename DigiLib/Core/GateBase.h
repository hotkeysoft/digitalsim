#pragma once

#include <set>
#include <map>
#include <memory>
#include "IOPin.h"
#include "IOConnection.h"
#pragma warning ( disable:26426 )

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
		typedef std::map<std::string, std::unique_ptr<IOPin> > IOPinMapType;
		typedef std::set<IOConnection> PinConnectionsType;
		typedef std::map<IOPin*, PinConnectionsType> ConnectedPinsType;
		typedef std::map<IOPin::IO_DIRECTION, std::map<IOPin::IO_DIRECTION, bool> > AllowedConnectionMapType;

		class DllExport GateBase
		{
		public:
			GateBase(const char* name);
			virtual ~GateBase() = default;
			GateBase(const GateBase&) = delete;
			GateBase& operator=(const GateBase&) = delete;
			GateBase(GateBase&&) = delete;
			GateBase& operator=(GateBase&&) = delete;

			std::string GetName() { return m_name; }
			std::string GetFullName();
			virtual void SetName(const char*);

			virtual GateBase* Clone(const char* name) = 0;

			virtual IOPin* GetPin(const char*  name);

			virtual PinConnectionsType& GetConnectedPins(const char* sourcePin);
			virtual PinConnectionsType& GetConnectedPins(IOPin* sourcePin);
			virtual ConnectedPinsType& GetConnectedPins() noexcept { return m_connectedToPins; }

			GateBase* GetParent() noexcept { return m_parent; }
			virtual void SetParent(GateBase* parent);

			virtual void ComputeState() noexcept(false) {};

			size_t GetInputCount() noexcept { return m_inputPins.size(); }
			size_t GetOutputCount() noexcept { return m_outputPins.size(); }

			const IOPinMapType& GetInputPins() noexcept { return m_inputPins; }
			const IOPinMapType& GetOutputPins() noexcept { return m_outputPins; }

			void ConnectPins(IOPin* source, IOPin* target);

		protected:
			virtual IOPin* AddInput(const char*  name, int8_t width = 1);
			virtual IOPin* AddOutput(const char*  name, int8_t width = 1, IOPin::IO_DIRECTION dir = IOPin::IO_DIRECTION::OUTPUT);

			std::string m_name;
			GateBase* m_parent;

			ConnectedPinsType m_connectedToPins;
			ConnectedPinsType m_connectedFromPins;

			IOPinMapType m_inputPins;
			IOPinMapType m_outputPins;

			static AllowedConnectionMapType m_insideInsideMap;
			static AllowedConnectionMapType m_insideParentMap;
			static AllowedConnectionMapType m_parentInsideMap;
			static void InitAllowedConnectionMaps();

			bool IsValidPinName(const char* name);
			void ValidatePinName(const char* name);
			void ValidatePinWidth(int8_t width);

			virtual bool IsValidGateName(const char* name);
			virtual void ValidateGateName(const char* name);
		};
	}
}