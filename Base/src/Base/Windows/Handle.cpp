#include "Base/Windows/Handle.h"

#include <cassert>
#include <system_error>

#include "Base/Exception.h"

namespace base
{
	namespace win
	{
		Handle::Handle() :
			m_handle(nullptr)
		{
		}

		Handle::Handle(HANDLE handle) :
			m_handle(handle)
		{
		}

		Handle::~Handle()
		{
			if (m_handle != nullptr)
				CloseHandle(m_handle);
		}

		Handle::Handle(Handle&& other) noexcept :
			m_handle(other.m_handle)
		{
			other.m_handle = nullptr;
		}

		bool Handle::wait(DWORD milliSeconds)
		{
			DWORD waited = WaitForSingleObject(m_handle, milliSeconds);
			if (waited == WAIT_FAILED)
				BASE_THROW(std::system_error(GetLastError(), std::system_category()));
			return waited != WAIT_TIMEOUT;
		}

		bool Handle::operator ==(nullptr_t) const
		{
			return m_handle == nullptr;
		}

		bool Handle::operator !=(nullptr_t) const
		{
			return m_handle != nullptr;
		}

		bool Handle::operator !() const
		{
			return m_handle == nullptr;
		}

		Handle::operator bool() const
		{
			return m_handle != nullptr;
		}

		Handle::operator HANDLE() const
		{
			return m_handle;
		}

		HANDLE* Handle::operator &()
		{
			assert(m_handle == nullptr);
			return &m_handle;
		}
	}
}
