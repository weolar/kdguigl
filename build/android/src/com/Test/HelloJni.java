/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.Test;

import android.app.Activity;
import android.content.Context;
import android.graphics.Canvas;
import android.view.View;
import android.widget.TextView;
import android.os.Bundle;
import android.os.Handler;

class MyView extends View {
	public MyView(Context context) {
		super(context);
	}

	protected void onDraw(Canvas canvas) {
		super.onDraw(canvas);
		OnDraw(canvas);
	}

	public native void OnDraw(Canvas canvas);
	
	public static void test() {
		
	}

	static {
		System.loadLibrary("skia");
		//System.loadLibrary("skiagl");
		System.loadLibrary("kdgui");
	}
}

public class HelloJni extends Activity {
	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		/*
		 * Create a TextView and set its content. the text is retrieved by
		 * calling a native function.
		 */
		// TextView tv = new TextView(this);
		// tv.setText( stringFromJNI() );
		// setContentView(tv);
		MyView.test();
		mHandler.sendEmptyMessageDelayed(0, 10000);
	}

	private Handler mHandler = new Handler() {
		public void handleMessage(android.os.Message msg) {
			MyView view = new MyView(HelloJni.this);
			setContentView(view);
		};
	};

	public native String unimplementedStringFromJNI();
}
