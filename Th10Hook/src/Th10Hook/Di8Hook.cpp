#include "Th10Hook/Di8Hook.h"

#include <Base/Error.h>

#include "Th10Hook/MyDirectInput8A.h"

namespace th
{
	Di8Hook g_di8Hook;

	Di8Hook::Di8Hook() :
		Singleton(this),
		m_directInput8CreateOrig(nullptr)
	{
	}

	void Di8Hook::attach(MyDetours& detours)
	{
		HMODULE dinput8Dll = GetModuleHandleW(L"dinput8.dll");
		if (dinput8Dll == nullptr)
			throw Error(GetLastError());
		m_directInput8CreateOrig = reinterpret_cast<DirectInput8Create_t>(GetProcAddress(dinput8Dll, "DirectInput8Create"));
		if (m_directInput8CreateOrig == nullptr)
			throw Error(GetLastError());

		detours.attach(reinterpret_cast<PVOID*>(&m_directInput8CreateOrig), DirectInput8CreateHook);
	}

	void Di8Hook::detach(MyDetours& detours)
	{
		detours.detach(reinterpret_cast<PVOID*>(&m_directInput8CreateOrig), DirectInput8CreateHook);
	}

	HRESULT Di8Hook::DirectInput8CreateHook(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter)
	{
		Di8Hook& instance = Di8Hook::GetInstance();
		return instance.directInput8CreateHook(hinst, dwVersion, riidltf, ppvOut, punkOuter);
	}

	HRESULT Di8Hook::directInput8CreateHook(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter)
	{
		HRESULT hr = m_directInput8CreateOrig(hinst, dwVersion, riidltf, ppvOut, punkOuter);
		if (SUCCEEDED(hr))
		{
			if (riidltf == IID_IDirectInput8A)
				*ppvOut = new MyDirectInput8A(reinterpret_cast<IDirectInput8A*>(*ppvOut));
		}
		return hr;
	}
}
