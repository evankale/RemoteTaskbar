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

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.TextView;

public class ListItemArrayAdapter extends ArrayAdapter
{
	private final Context context;
	private final ArrayList<Task> tasks;
	private final HashMap<View, Task> closeBtnMap;
	private final View.OnClickListener closeBtnClickListener;

	public ListItemArrayAdapter(Context context, ArrayList<Task> tasks)
	{
		super(context, R.layout.list_item, tasks);
		this.context = context;
		this.tasks = tasks;

		closeBtnMap = new HashMap<View, Task>();

		closeBtnClickListener = new View.OnClickListener()
		{
			@Override
			public void onClick(View v)
			{
				Task task = closeBtnMap.get(v);
				if (task != null)
				{
					TasksActivity.instance.clientThread.signalWindowClose(task.hwnd);
				}
			}
		};
	}

	@Override
	public View getView(int position, View convertView, ViewGroup parent)
	{
		LayoutInflater inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);

		View rowView = inflater.inflate(R.layout.list_item, parent, false);
		TextView firstLine = (TextView) rowView.findViewById(R.id.firstLine);
		TextView secondLine = (TextView) rowView.findViewById(R.id.secondLine);
		ImageView imageView = (ImageView) rowView.findViewById(R.id.icon);

		ImageView closeTaskBtn = (ImageView) rowView.findViewById(R.id.closeTaskButton);

		firstLine.setText(((Task) getItem(position)).title);
		closeTaskBtn.setOnClickListener(closeBtnClickListener);

		Task task = tasks.get(position);
		closeBtnMap.put(closeTaskBtn, task);
		int hicon = task.hicon;

		if (hicon != 0)
		{
			imageView.setImageBitmap(TasksActivity.iconMap.get(hicon));
		}

		String hwndString = "0x" + Integer.toHexString(tasks.get(position).hwnd).toUpperCase();

		secondLine.setText(hwndString);

		return rowView;
	}
}
