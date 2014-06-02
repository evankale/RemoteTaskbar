RemoteTaskbar
=============

Summary: Controls Windows tasks on a mobile device.

Description:
This project contains a native Windows application (RemoteTaskbarServer), and an Android application (RemoteTaskbarClient).
These two applications communicate using TCP/IP, allowing a user remotely to view, open, and close tasks of a Windows macine on an Android device.

Build:
- RemoteTaskbarServer was built with Visual Studio 2010
  - When building this, it is mandatory that the target platform matches the operating system (ie, Win32 or x64)
- RemoteTaskbarClient was built with Android SDK and Eclipse IDE

Run:
1) Connect windows machine and android device to same network
2) Run  RemoteTaskbarServer.exe on Windows machine
3) Press "Connect" on server application
4) Run Remote Taskbar Client on android device
5) Enter the IP address of the windows machine; leave port as 9999
6) Press "Connect" on client application
7) Server application should say "Connected!" and a list of current tasks should be displayed on android device