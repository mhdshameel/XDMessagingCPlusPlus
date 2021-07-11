#pragma once
#ifndef ZWMESSAGING_COMMON
#define ZWMESSAGING_COMMON

#include <atlstr.h>

#define ZCString CAtlString

namespace ZWMessaging
{
	ZCString GetChannelKey(ZCString channelName);
}

#endif // ZWMESSAGING_COMMON