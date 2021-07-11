#include "Common.h"

ZCString ZWMessaging::GetChannelKey(ZCString channelName)
{
	ZCString str;
	str.Format(_T("ZW.Messaging.%s"), channelName);
	return str;
}