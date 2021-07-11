# XDMessagingCPlusPlus
This library provides an easy-to-use, zero configuration solution to inter-process communication between processes and applications.

The library allows the use of user-defined channels through which messages may be sent and received. Any application can send a message to any channel, but it must register as a listener with the channel in order to receive. In this way developers can quickly and programmatically devise how their applications will communicate with each other best to work in harmony.

This uses the Windows messages and the WM_COPYDATA
1. Uses the WM_COPYDATA Windows Message to copy data between applications. The broadcaster implementation sends the Windows Messages directly to a hidden window on the listener instance, which dispatches the MessageReceived event with the copied data.
2. Listeners in this mode must be created on the UI Thread.
3. Channels are created by adding/removing Windows properties.
4. This does not work for Windows Services, Console apps, or other applications without a message pump.

This was written based on the XDMessaging.Net developed by TheCodeKing using c# for .net based applications. In case your C++ native application wants to communicate with a C# application or process, you can make use of the XDMessaging.Net.
