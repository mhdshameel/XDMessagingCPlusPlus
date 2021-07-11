#pragma once
#ifndef ZWMESSAGING_BROADCAST
#define ZWMESSAGING_BROADCAST

#include "Common.h"

namespace ZWMessaging
{
	class ZWBroadcast 
	{
	public:
		ZWBroadcast();
		~ZWBroadcast();

		BOOL SendToChannel(ZCString channel, ZCString message);
		void AddToFilteredWnds(HWND wnd);

	private:
		std::vector<HWND> vFilteredWnds;
		void ClearFilteredWindows();
		std::vector<HWND> getFilteredWindows();
	};

}

#endif // !ZWMESSAGING_BROADCAST
