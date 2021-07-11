#include "Common.h"
#include <future>
#include "ZWListener.h"

using namespace ZWMessaging;

#define ZWLISTENER_WINDOW_CLASS_NAME _T("WINDOW_CLASS_FOR_ZWMESSAGING")
#define ZWLISTENER_WINDOW_NAME _T("ZWMessaging.ZWListener.Window")

LRESULT CALLBACK WndMsgHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int ZWListener::count = 0;
std::vector<MessageReceiveHandler> ZWListener::vHandlers;
std::vector<std::future<void>> ZWListener::futures_stack;

ZWListener::ZWListener()
{
	WNDCLASSEX wx = {};
	wx.cbSize = sizeof(WNDCLASSEX);

	// Callback function to handle messages
	wx.lpfnWndProc = WndMsgHandler;
	//wx.hInstance = ::AfxGetInstanceHandle ( ) ;
	wx.hInstance = ::GetModuleHandle(NULL);
	wx.lpszClassName = ZWLISTENER_WINDOW_CLASS_NAME;

	if (!RegisterClassEx(&wx))
	{
		return;
	}

	//HWND agent_message_window = CreateWindowEx ( 0 , WINDOW_CLASS_NAME , _T("dummy_name") , 0 , 10 , 10 , 200 , 200 , GetConsoleWindow() , NULL , NULL , NULL ) ;
	ZCString WndName;
	WndName.Format(_T("%s%d"), ZWLISTENER_WINDOW_NAME, count);

	HWND agent_message_window = CreateWindowEx(0, ZWLISTENER_WINDOW_CLASS_NAME, WndName.GetBuffer(), 0, 0, 0, 0, 0, NULL, NULL, NULL, NULL);

	mhWnd = agent_message_window;
	count++;

	hMsgLoopThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ListenerMessageLoop, mhWnd, 0, NULL);

	/*
	hMsgLoopThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ListenerMessageLoop, mhWnd, CREATE_SUSPENDED, &ThreadId);
	if (!hMsgLoopThread)
	{
	auto err = GetLastError();
	}

	if (SetThreadToken(&hMsgLoopThread, hUserTok))
	{
	DWORD thread_suspended_count = ResumeThread(hMsgLoopThread);

	if (thread_suspended_count == (DWORD)0)
	{

	}
	}
	else
	{
	auto err = GetLastError();
	}

	RevertToSelf();
	if (hUserTok)
	CloseHandle(hUserTok);
	*/
}

ZWListener::~ZWListener()
{
	if (mhWnd)
	{
		SendMessage(mhWnd, WM_DESTROY, 0, 0);
		UnregisterClass(ZWLISTENER_WINDOW_CLASS_NAME, NULL);
		mhWnd = NULL;
	}
}

HWND ZWListener::GetWindowHandle()
{
	return mhWnd;
}

BOOL ZWListener::RegisterChannel(ZCString channelName)
{
	if (!mhWnd)
		return FALSE;
	return SetProp(this->mhWnd, GetChannelKey(channelName), this->mhWnd);
}

void ZWListener::UnRegisterChannel(ZCString channelName)
{
	if (!mhWnd)
		return;
	RemoveProp(this->mhWnd, GetChannelKey(channelName));
}

void ZWListener::ListenerMessageLoop(HWND hWnd)
{
	MSG msg;
	try
	{
		while (GetMessage(&msg, hWnd, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	catch (const std::exception& e)
	{
		printf("Exception in ListenerMessageLoop : %s\n", e.what());
	}
	catch (...)
	{
		printf("Unkown exception in ListenerMessageLoop\n");
	}
}


void ZWListener::RegisterMessageReceiveEventHandler(MessageReceiveHandler msgReceiveHandler)
{
	if (msgReceiveHandler)
	{
		vHandlers.push_back(msgReceiveHandler);
	}
}

void ZWListener::DispatchMessages(ZCString channel, ZCString message)
{
	for each (auto var in vHandlers)
	{
		auto f = std::async(std::launch::async, [var, channel, message] {
			//we can add a mutex here if we want
			var(channel, message);
		}
		);
		ZWListener::MoveToFutureStack(f);
	}
}

void ZWListener::UnRegisterMessageReceiveEventHandler(MessageReceiveHandler msgReceiveHandler)
{
	if (msgReceiveHandler)
	{
		if (vHandlers.size() > 0)
		{
			for(std::vector<MessageReceiveHandler>::iterator it = vHandlers.begin(); it != vHandlers.end(); it++)
			{
				if (*it == msgReceiveHandler)
				{
					vHandlers.erase(it);
					break;
				}
			}
		}
	}
}

void ZWListener::MoveToFutureStack(std::future<void> &f)
{
	futures_stack.push_back(std::move(f));
}

LRESULT CALLBACK WndMsgHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_COPYDATA:
		if(lParam)
		{
			PCOPYDATASTRUCT pdata = (PCOPYDATASTRUCT)lParam;
			ZCString receivedMsg = (TCHAR*)pdata->lpData, channel, message;
			int nTokenPos = 0;
			channel = receivedMsg.Tokenize(_T(":"), nTokenPos);
			message = receivedMsg.Right(receivedMsg.GetLength() - nTokenPos);
			ZWListener::DispatchMessages(channel, message);
		}
		break;

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
		break;
	}
	return 0;
}

