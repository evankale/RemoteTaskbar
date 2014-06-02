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

import java.util.ArrayList;
import java.util.HashMap;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.GridView;

public class TasksActivity extends Activity
{
	public static TasksActivity instance;
	public static HashMap<Integer, Bitmap> iconMap;
	public ClientThread clientThread;

	private static ArrayList<Task> taskList;
	private ListItemArrayAdapter listViewAdapter;
	private GridView gridView;

	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		instance = this;
		taskList = new ArrayList<Task>();
		iconMap = new HashMap<Integer, Bitmap>();

		// Make this activity fullscreen and prevent the device from going to sleep
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		getWindow().getDecorView().setBackgroundColor(Color.BLACK);
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

		setContentView(R.layout.list_gridview);

		clientThread = new ClientThread();
		new Thread(clientThread).start();

		gridView = (GridView) findViewById(R.id.gridview);

		gridView.setOnItemClickListener(new OnItemClickListener()
		{
			@Override
			public void onItemClick(AdapterView<?> parent, final View view, int position, long id)
			{
				clientThread.signalWindowOpen(taskList.get(position).hwnd);
			}
		});

		listViewAdapter = new ListItemArrayAdapter(this, taskList);
		gridView.setAdapter(listViewAdapter);
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event)
	{
		if ((keyCode == KeyEvent.KEYCODE_BACK))
		{
			clientThread.closeConnection();
		}
		return super.onKeyDown(keyCode, event);
	}

	public void updateItems()
	{
		runOnUiThread(new Runnable()
		{
			@Override
			public void run()
			{
				listViewAdapter.notifyDataSetChanged();
			}
		});
	}

	public void removeTaskByHwnd(final int hwnd)
	{
		runOnUiThread(new Runnable()
		{
			@Override
			public void run()
			{
				Task t = getTaskByHwnd(hwnd);
				if (t != null)
					listViewAdapter.remove(t);
			}
		});
	}

	public void addTask(final Task task)
	{
		runOnUiThread(new Runnable()
		{
			@Override
			public void run()
			{
				listViewAdapter.add(task);
			}
		});
	}

	public void clearTaskList()
	{
		runOnUiThread(new Runnable()
		{
			@Override
			public void run()
			{
				listViewAdapter.clear();
			}
		});
	}

	public Task getTaskByHwnd(int hwnd)
	{
		for (Task t : taskList)
		{
			if (t.hwnd == hwnd)
			{
				return t;
			}
		}
		return null;
	}
}
