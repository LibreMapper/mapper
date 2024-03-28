/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2019 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

package org.openorienteering.mapper;

// Until Qt Creator and androiddeployqt fully support per-flavor app IDs,
// we must change the ID in AndroidManifest.xml, and thus the BuildConfig
// and R classes are generated in the MAPPER_APP_ID namespace.
import @MAPPER_APP_ID@.BuildConfig;
import @MAPPER_APP_ID@.R;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.os.IBinder;


/**
 * A foreground service which keeps Mapper alive enough
 * to save the modified map and to continue GPX recording.
 */
public class MapperService extends Service
{
	static private int NOTIFICATION_ID = 1;
	private static NotificationChannel channel;
	
	private String makeNotificationChannel()
	{
		String channel_id = BuildConfig.APPLICATION_ID + ".channel1";
		NotificationChannel channel = new NotificationChannel(channel_id,
		                                                      getString(R.string.long_app_name),
		                                                      NotificationManager.IMPORTANCE_LOW);
		((NotificationManager)this.getSystemService(Context.NOTIFICATION_SERVICE))
			.createNotificationChannel(channel);
		return channel_id;
	}
	
	private Notification.Builder makeNotificationBuilder()
	{
		if (Build.VERSION.SDK_INT < 26)
		{
			return new Notification.Builder(this);
		}
		
		String channel_id = makeNotificationChannel();
		return new Notification.Builder(this, channel_id);
	}
	
	@Override
	public int onStartCommand(Intent intent, int flags, int startId)
	{
		String message = intent.getStringExtra("message");
		
		Intent notificationIntent = new Intent(this, MapperActivity.class);
		PendingIntent pendingIntent = PendingIntent.getActivity(this, 0, notificationIntent, 0);
		Notification notification = makeNotificationBuilder()
			.setSmallIcon(android.R.drawable.ic_dialog_map)
			.setContentTitle(getString(R.string.app_name))
			.setContentText(message)
			.setContentIntent(pendingIntent)
			.build();
		
		startForeground(NOTIFICATION_ID, notification);
		
		return START_NOT_STICKY;
	}
	
	@Override
	public IBinder onBind (Intent intent)
	{
		return null;
	}
	
}
