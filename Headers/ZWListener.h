#pragma once
#ifndef ZWMESSAGING_LISTENER
#define ZWMESSAGING_LISTENER

#include <tchar.h>
#include <vector>
#include <map>
#include <future>

namespace ZWMessaging
{
	
	///Message receive event handler's parameters should be Unicode (UTF-16) based 
	///CString is a compatible option
	typedef void(*MessageReceiveHandler)(ZCString, ZCString);

	class ZWListener
	{
	public:
		ZWListener();
		~ZWListener();

		HWND GetWindowHandle();

		static std::vector<MessageReceiveHandler> vHandlers;

		static void DispatchMessages(ZCString channel, ZCString message);

		//If the function fails, the return value is zero. To get extended error information, call GetLastError.
		BOOL RegisterChannel(ZCString channelName);

		void UnRegisterChannel(ZCString channelName);

		void RegisterMessageReceiveEventHandler(MessageReceiveHandler msgReceiveHandler);

		void UnRegisterMessageReceiveEventHandler(MessageReceiveHandler msgReceiveHandler);
		
	private:
		static int count;
		static std::vector<std::future<void>> futures_stack;
		HWND mhWnd = NULL;
		HANDLE hMsgLoopThread = NULL;
		static void ListenerMessageLoop(HWND hWnd);
		static void MoveToFutureStack(std::future<void> &f);
	};
}

#endif //ZWMESSAGING_LISTENER