#pragma once

#include <CppUnitTest.h>
#include "GateBase.h"
#include "CompositeGate.h"
#include "LogicTools.h"

namespace Microsoft {
	namespace VisualStudio {
		namespace CppUnitTestFramework {
			template<> std::wstring ToString<IOPin::IO_DIRECTION>(const IOPin::IO_DIRECTION& t) { return t == IOPin::INPUT ? L"INPUT" : L"OUTPUT"; }
			template<> std::wstring ToString<IOPin::IO_STATE>(const IOPin::IO_STATE& t)
			{
				switch (t)
				{
				case IOPin::HI: return L"HI";
				case IOPin::HI_Z: return L"HI_Z";
				case IOPin::LOW: return L"LOW";
				case IOPin::UNDEF: return L"UNDEF";
				}
				return L"undefined state";
			}
			template<> std::wstring ToString<GateBase>(GateBase* q) 
			{ 
				RETURN_WIDE_STRING(q);
			}
			template<> std::wstring ToString<CompositeGate>(CompositeGate* q)
			{
				RETURN_WIDE_STRING(q);
			}
			template<> std::wstring ToString<LogicTools::IOStateList>(const LogicTools::IOStateList& t)
			{
				std::wostringstream os;
				for (auto state : t)
				{
					switch (state)
					{
					case IOPin::HI_Z:
						os << L"HI_Z";
						break;
					case IOPin::HI:
						os << L"HI";
						break;
					case IOPin::LOW:
						os << L"LOW";
						break;
					case IOPin::UNDEF:
						os << L"UNDEF";
						break;
					default:
						os << L"*INVALID*";
					}
					os << ",";
				}
				std::wstring ret = os.str();
				if (ret.back() == L',')
				{
					ret.erase(ret.size() - 1);
				}
				return ret;
			}
		}
	}
}

namespace UnitTests
{
	CompositeGate* BuildDecoder();
}
