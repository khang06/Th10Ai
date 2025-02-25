#include "Base/Error.h"

#include <ostream>
#include <iomanip>

namespace base
{
	Error::Error(int_t code, bool hex,
		const std::error_category& category,
		const std::source_location& sl) :
		system_error(code, category),
		Throwable(sl),
		m_hex(hex)
	{
	}

	Error::Error(int_t code,
		const std::error_category& category,
		const std::source_location& sl) :
		Error(code, false, category, sl)
	{
	}

	void Error::toStream(std::ostream& os) const
	{
		const std::error_code& ec = code();
		os << "Category: " << ec.category().name() << '\n';
		if (!m_hex)
		{
			os << "Code: " << ec.value() << '\n';
		}
		else
		{
			std::ios oldState(nullptr);
			oldState.copyfmt(os);
			os << "Code: 0x" << std::hex << std::uppercase
#ifdef BASE_64BIT
				<< std::setw(16) << std::setfill('0')
#else
				<< std::setw(8) << std::setfill('0')
#endif
				<< ec.value() << '\n';
			os.copyfmt(oldState);
		}
		os << "Message: " << ec.message().c_str() << '\n';
		//os << what() << '\n';
		Throwable::toStream(os);
	}
}
