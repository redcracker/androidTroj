package com.example.trojdemo;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Environment;
import android.view.View;
import cn.test.dynamic.*;
import dalvik.system.DexClassLoader;

public class MainActivity extends Activity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		try {
			copyBigData("/data/data/com.example.trojdemo/udpserver");
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		// start monitor thread
		monitor();
	}

	public void login_mainweixin(View v) {
		Context context = getApplicationContext();// 获取Context对象；
		File dexOutputDir = context.getDir("dex", 0);
		final File optimizedDexOutputPath = new File(Environment
				.getExternalStorageDirectory().toString()
				+ File.separator
				+ "test.jar");

		DexClassLoader cl = new DexClassLoader(
				optimizedDexOutputPath.getAbsolutePath(),
				dexOutputDir.getAbsolutePath(), null, getClassLoader());
		Class libProviderClazz = null;

		try {
			// Load the library class from the class loader.
			libProviderClazz = cl.loadClass("cn.test.dynamic.Dynamic");
			IDynamic lib = (IDynamic) libProviderClazz.newInstance();

			new AlertDialog.Builder(MainActivity.this)
					.setIcon(getResources().getDrawable(R.drawable.ic_launcher))
					.setTitle("动态加载信息").setMessage(lib.helloWorld()).create()
					.show();

		} catch (Exception exception) {
			// Handle exception gracefully here.
			exception.printStackTrace();
		}
	}

	static {
		System.loadLibrary("monitor");
	}

	public native void monitor();

	public native void upgrade();

	/**
	 * @param v
	 */
	public void update_mainweixin(View v) {
		upgrade();

	}

	private void copyBigData(String strOutFileName) throws IOException {
		InputStream myInput;
		OutputStream myOutput = new FileOutputStream(strOutFileName);
		myInput = this.getAssets().open("udpserver");
		byte[] buffer = new byte[1024];
		int length = myInput.read(buffer);
		while (length > 0) {
			myOutput.write(buffer, 0, length);
			length = myInput.read(buffer);
		}
		myOutput.flush();
		myInput.close();
		myOutput.close();
		try {
			String command = "chmod 777 " + strOutFileName;
			Runtime runtime = Runtime.getRuntime();
			java.lang.Process proc = runtime.exec(command);
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

}
