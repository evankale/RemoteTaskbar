/*
* Copyright (c) 2014 Evan Kale
* Email: EvanKale91@gmail.com
* Website: www.ISeeDeadPixel.com
* 
* This file is part of RemoteTaskbar.
*
* RemoteTaskbar is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

package com.isdp.remotetaskbarclient;

import java.io.BufferedOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.net.InetAddress;
import java.net.Socket;
import java.nio.ByteBuffer;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Environment;
import android.util.Log;

public class ClientThread implements Runnable
{
	// signals from the client to server
	public static final int CLIENT_REQUEST_ACTIVE_TASK_LIST = 1001;
	public static final int CLIENT_SIGNAL_WINDOW_OPEN = 1002;
	public static final int CLIENT_SIGNAL_WINDOW_CLOSE = 1003;

	// signals from the server to client
	public static final int SERVER_RESPONSE_ACTIVE_TASK_LIST = 2001;
	public static final int SERVER_RESPONSE_SEND_ICON = 2002;
	public static final int SERVER_RESPONSE_ADD_TASK = 2003;
	public static final int SERVER_RESPONSE_REMOVE_TASK = 2004;
	public static final int SERVER_RESPONSE_UPDATE_TASK = 2005;

	private Socket socket;

	@Override
	public void run()
	{
		try
		{
			InetAddress serverAddr = InetAddress.getByName(LoginActivity.serverIP);
			socket = new Socket(serverAddr, LoginActivity.serverPort);

			requestActiveTaskList();

			while (true)
			{
				DataInputStream inputStream = new DataInputStream(socket.getInputStream());

				int serverResponseType = inputStream.readInt();

				Log.d("ClientThread", "Response Type : " + serverResponseType + " Avail : " + inputStream.available());

				switch (serverResponseType)
				{
					case SERVER_RESPONSE_ACTIVE_TASK_LIST:

						Log.d("ClientThread", "SERVER_RESPONSE_ACTIVE_TASK_LIST");

						TasksActivity.instance.clearTaskList();

						int numTasks = inputStream.readInt();

						for (int i = 0; i < numTasks; ++i)
						{
							int hwnd = inputStream.readInt();
							int hicon = inputStream.readInt();
							int titleLength = inputStream.readInt();
							byte titleBuffer[] = new byte[titleLength];
							inputStream.readFully(titleBuffer, 0, titleLength);
							String title = new String(titleBuffer);
							TasksActivity.instance.addTask(new Task(hwnd, hicon, title));
						}

						TasksActivity.instance.updateItems();

						break;

					case SERVER_RESPONSE_SEND_ICON:

						Log.d("ClientThread", "SERVER_RESPONSE_SEND_ICON");

						int hicon = inputStream.readInt();
						int pngSize = inputStream.readInt();
						byte pngBuffer[] = new byte[pngSize];
						inputStream.readFully(pngBuffer, 0, pngSize);
						Bitmap bmp = BitmapFactory.decodeByteArray(pngBuffer, 0, pngSize);

						/*if (bmp == null)
						{
							System.out.println(hicon + " " + pngSize);
							dumpByteArray(pngBuffer);
						}*/

						TasksActivity.instance.iconMap.put(hicon, bmp);

						break;

					case SERVER_RESPONSE_ADD_TASK:

						Log.d("ClientThread", "SERVER_RESPONSE_ADD_TASK");

						int hwnd = inputStream.readInt();
						hicon = inputStream.readInt();
						int titleLength = inputStream.readInt();
						byte titleBuffer[] = new byte[titleLength];
						inputStream.readFully(titleBuffer, 0, titleLength);
						String title = new String(titleBuffer);
						TasksActivity.instance.addTask(new Task(hwnd, hicon, title));

						TasksActivity.instance.updateItems();

						break;

					case SERVER_RESPONSE_REMOVE_TASK:

						Log.d("ClientThread", "SERVER_RESPONSE_REMOVE_TASK");

						hwnd = inputStream.readInt();

						TasksActivity.instance.removeTaskByHwnd(hwnd);
						TasksActivity.instance.updateItems();

						break;

					case SERVER_RESPONSE_UPDATE_TASK:

						Log.d("ClientThread", "SERVER_RESPONSE_UPDATE_TASK");

						hwnd = inputStream.readInt();
						titleLength = inputStream.readInt();
						titleBuffer = new byte[titleLength];
						inputStream.readFully(titleBuffer, 0, titleLength);
						title = new String(titleBuffer);

						Task t = TasksActivity.instance.getTaskByHwnd(hwnd);
						if (t != null)
							t.title = title;

						TasksActivity.instance.updateItems();

						break;
				}
			}
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}

	public void requestActiveTaskList()
	{
		try
		{
			DataOutputStream outputStream = new DataOutputStream(socket.getOutputStream());
			outputStream.writeInt(CLIENT_REQUEST_ACTIVE_TASK_LIST);
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}

	public void signalWindowOpen(int hwnd)
	{
		try
		{
			BufferedOutputStream outputStream = new BufferedOutputStream(socket.getOutputStream());
			byte buffer[] = new byte[8];
			ByteBuffer byteBuffer = ByteBuffer.wrap(buffer);
			byteBuffer.putInt(CLIENT_SIGNAL_WINDOW_OPEN);
			byteBuffer.putInt(hwnd);
			outputStream.write(buffer);
			outputStream.flush();
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}

	public void signalWindowClose(int hwnd)
	{
		try
		{
			BufferedOutputStream outputStream = new BufferedOutputStream(socket.getOutputStream());
			byte buffer[] = new byte[8];
			ByteBuffer byteBuffer = ByteBuffer.wrap(buffer);
			byteBuffer.putInt(CLIENT_SIGNAL_WINDOW_CLOSE);
			byteBuffer.putInt(hwnd);
			outputStream.write(buffer);
			outputStream.flush();
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}

	public void closeConnection()
	{
		try
		{
			socket.close();
		}
		catch (Exception e)
		{
		}
	}

	private void dumpByteArray(byte[] array)
	{
		try
		{
			String path = Environment.getExternalStorageDirectory().getAbsolutePath() + "/dumpedBytes.png";
			File file = new File(path);

			if (!file.exists())
			{
				file.createNewFile();
			}

			FileOutputStream stream = new FileOutputStream(path);
			stream.write(array);
			stream.close();
		}
		catch (Exception e1)
		{
			e1.printStackTrace();
		}
	}

}
