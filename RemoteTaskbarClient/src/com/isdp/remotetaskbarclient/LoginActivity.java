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

import android.app.Activity;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.graphics.Color;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.EditText;

public class LoginActivity extends Activity
{
	public static String serverIP;
	public static int serverPort;

	private static final String PREF_SERVER_IP = "server_ip";
	private static final String PREF_SERVER_PORT = "server_port";

	private Button connectBtn;
	private EditText ipText;
	private EditText portText;

	private SharedPreferences lastUsedPreference;
	private Editor lastUsedPreferenceEditor;

	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);

		// Make this activity fullscreen and prevent the device from going to sleep
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		getWindow().getDecorView().setBackgroundColor(Color.BLACK);
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

		lastUsedPreference = getSharedPreferences("last_used", 0);
		lastUsedPreferenceEditor = lastUsedPreference.edit();

		setContentView(R.layout.login);

		connectBtn = (Button) findViewById(R.id.connectButton);
		ipText = (EditText) findViewById(R.id.ipText);
		portText = (EditText) findViewById(R.id.portText);

		serverIP = lastUsedPreference.getString(PREF_SERVER_IP, "");
		serverPort = lastUsedPreference.getInt(PREF_SERVER_PORT, 9999);

		ipText.setText(serverIP);
		portText.setText(serverPort + "");

		connectBtn.setOnClickListener(new OnClickListener()
		{
			@Override
			public void onClick(View v)
			{
				try
				{
					serverIP = ipText.getText().toString();
					serverPort = Integer.parseInt(portText.getText().toString());
					lastUsedPreferenceEditor.putString(PREF_SERVER_IP, serverIP);
					lastUsedPreferenceEditor.putInt(PREF_SERVER_PORT, serverPort);
					lastUsedPreferenceEditor.commit();
					gotoTaskActivity();
				}
				catch (Exception e)
				{
				}
			}
		});
	}

	private void gotoTaskActivity()
	{
		Intent intent = new Intent(this, TasksActivity.class);
		startActivity(intent);
	}
}
