#include "Common.h"
#include <iostream>
#include <vector>
#include "ZWBroadcast.h"

using namespace ZWMessaging;

BOOL CALLBACK EnumWndProc(HWND hWnd, LPARAM lParam);
BOOL CALLBACK EnumWndProcDummy(HWND hWnd, LPARAM lParam);

ZWBroadcast::ZWBroadcast()
{
}

ZWBroadcast::~ZWBroadcast()
{
}

BOOL ZWBroadcast::SendToChannel(ZCString channel, ZCString message)
{
	DWORD dwRetrunCode = ERROR_SUCCESS;
	if (vFilteredWnds.size() == 0 || !::IsWindow(vFilteredWnds.at(0))) 
	{
		_tprintf(_T("vFilteredWnds is 0 or oth window is nt available anymore. msg : %s"), message);

		ClearFilteredWindows();
		std::pair<ZCString, ZWBroadcast*> params = std::make_pair(channel, this);
		if (!EnumWindows(EnumWndProc, (LPARAM)&params))
		{
			_tprintf(_T("EnumWindows failed : %d"), GetLastError());
		}
	}
#if 0
	else 
	{
		//for testing purposes only. Need to check if enumwindows in another desktop is working fine
		if (!EnumWindows(EnumWndProcDummy, NULL))
		{
			LogCritical(_T("EnumChildWindows for EnumWndProcDummy failed : %d"), GetLastError());
		}
	}
#endif
	
	ZCString data;
	data.Format(_T("%s:%s"), channel, message);
	//CStringA data = CStringA(channel) + ":" + CStringA(message);
	
	size_t dataSize = (data.GetLength() + 1) * sizeof(TCHAR);
	
	void * pData = CoTaskMemAlloc(dataSize);

	if (!pData)
	{
		_tprintf(_T("CoTaskMemAlloc failed. there is insufficient memory available"));
		return FALSE;
	}

	CopyMemory(pData, data.GetBuffer(), dataSize);

	COPYDATASTRUCT dataStruct;
	dataStruct.cbData = dataSize;
	//dataStruct.cbData = data.GetLength();
	dataStruct.dwData = NULL;
	dataStruct.lpData = pData;

	auto vec = getFilteredWindows();

	if (vec.size() > 1)
	{
		_tprintf(_T("Filtered windows to receive msg count : %d"), vec.size());
	} 
	else if (vec.size() == 0)
	{
		_tprintf(_T("Filtered windows is ZERO. \'%s\' msg will not be sent"), message);
	}

	//revert
	//LogCritical(_T("Size : %d ; About to send : %s"), vec.size(), message);

	for each (auto uiHandle in vec)
	{
		if (SendMessageTimeout(uiHandle, WM_COPYDATA, NULL, (LPARAM)&dataStruct, SMTO_ABORTIFHUNG, 1000, NULL) == 0)
		{
			dwRetrunCode = GetLastError();
			if (dwRetrunCode == ERROR_TIMEOUT)
			{
				_tprintf(_T("%s: Timed out."), __FUNCTIONW__);
			}
			else
			{
				_tprintf(_T("%s: error in sendmessage : %d"), __FUNCTIONW__, dwRetrunCode);
			}
		}
	}

	CoTaskMemFree(pData);

	return (ERROR_SUCCESS == dwRetrunCode);
}

BOOL CALLBACK EnumWndProc(HWND hWnd, LPARAM lParam)
{
	auto * params = (std::pair<ZCString, ZWBroadcast*> *)lParam;
	ZCString channel = params->first;
	ZWBroadcast* pbroadcast = params->second;
	HANDLE handle = GetProp(hWnd, GetChannelKey(channel.GetBuffer()));
	if (handle)
	{
		pbroadcast->AddToFilteredWnds(hWnd);
	}
	return TRUE;
}

BOOL CALLBACK EnumWndProcDummy(HWND hWnd, LPARAM lParam)
{
	return TRUE;
}

void ZWBroadcast::ClearFilteredWindows()
{
	vFilteredWnds.clear();
}

void ZWBroadcast::AddToFilteredWnds(HWND wnd)
{
	vFilteredWnds.push_back(wnd);
}

std::vector<HWND> ZWBroadcast::getFilteredWindows()
{
	return vFilteredWnds;
}
