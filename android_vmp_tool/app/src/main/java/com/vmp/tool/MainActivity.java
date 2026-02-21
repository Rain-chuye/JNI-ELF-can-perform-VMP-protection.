package com.vmp.tool;

import android.app.Activity;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.provider.Settings;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;

public class MainActivity extends Activity {

    static {
        System.loadLibrary("vmp_engine");
    }

    private Uri selectedUri = null;
    private TextView tvStatus;
    private Button btnProtect;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        tvStatus = findViewById(R.id.tv_status);
        btnProtect = findViewById(R.id.btn_protect);
        Button btnSelect = findViewById(R.id.btn_select);

        checkPermissions();

        btnSelect.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
                intent.setType("*/*");
                startActivityForResult(intent, 1);
            }
        });

        btnProtect.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (selectedUri != null) {
                    processFile(selectedUri);
                } else {
                    Toast.makeText(MainActivity.this, "Please select a file first", Toast.LENGTH_SHORT).show();
                }
            }
        });
    }

    private void checkPermissions() {
        // Request legacy storage permissions for Android 10 and below
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M && Build.VERSION.SDK_INT < Build.VERSION_CODES.R) {
            if (checkSelfPermission(android.Manifest.permission.READ_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
                requestPermissions(new String[]{
                        android.Manifest.permission.READ_EXTERNAL_STORAGE,
                        android.Manifest.permission.WRITE_EXTERNAL_STORAGE
                }, 100);
            }
        }

        // Android 11+ (API 30+) MANAGE_EXTERNAL_STORAGE permission
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            if (!Environment.isExternalStorageManager()) {
                Toast.makeText(this, "Android 11+ requires All Files Access Permission", Toast.LENGTH_LONG).show();
                try {
                    Intent intent = new Intent(Settings.ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION);
                    intent.addCategory("android.intent.category.DEFAULT");
                    intent.setData(Uri.parse(String.format("package:%s", getPackageName())));
                    startActivityForResult(intent, 101);
                } catch (Exception e) {
                    Intent intent = new Intent();
                    intent.setAction(Settings.ACTION_MANAGE_ALL_FILES_ACCESS_PERMISSION);
                    startActivityForResult(intent, 101);
                }
            }
        }
    }

    private void processFile(Uri uri) {
        try {
            // 1. Copy URI content to a temp file in internal storage to bypass SAF restrictions for NDK
            File cacheDir = getExternalCacheDir();
            if (cacheDir == null) cacheDir = getCacheDir();

            File tempIn = new File(cacheDir, "input.so");
            File tempOut = new File(cacheDir, "output.so");

            InputStream is = getContentResolver().openInputStream(uri);
            if (is == null) throw new Exception("Could not open URI stream");

            OutputStream os = new FileOutputStream(tempIn);
            byte[] buffer = new byte[8192];
            int read;
            while ((read = is.read(buffer)) != -1) {
                os.write(buffer, 0, read);
            }
            is.close();
            os.close();

            Log.i("VMP", "Temp input file prepared: " + tempIn.length() + " bytes");

            // 2. Protect the temp file via JNI
            boolean success = protectSo(tempIn.getAbsolutePath(), tempOut.getAbsolutePath());

            if (success && tempOut.exists()) {
                // 3. Save the result back to public storage (/sdcard/VMP_Protected.so)
                // For Android 11+, we rely on MANAGE_EXTERNAL_STORAGE to write directly to /sdcard/
                File finalDir = new File(Environment.getExternalStorageDirectory(), "VMP");
                if (!finalDir.exists()) finalDir.mkdirs();

                File finalFile = new File(finalDir, "protected_" + System.currentTimeMillis() + ".so");

                copyFile(tempOut, finalFile);

                Toast.makeText(this, "Success! Saved to: " + finalFile.getAbsolutePath(), Toast.LENGTH_LONG).show();
                tvStatus.setText("Protected: " + finalFile.getAbsolutePath());

                // Cleanup
                tempIn.delete();
                tempOut.delete();
            } else {
                Toast.makeText(this, "Protection Failed! Check Logcat for details.", Toast.LENGTH_LONG).show();
            }

        } catch (Exception e) {
            Log.e("VMP", "Error: " + e.getMessage());
            Toast.makeText(this, "Error: " + e.getMessage(), Toast.LENGTH_LONG).show();
        }
    }

    private void copyFile(File src, File dst) throws Exception {
        FileInputStream in = new FileInputStream(src);
        FileOutputStream out = new FileOutputStream(dst);
        byte[] buf = new byte[8192];
        int len;
        while ((len = in.read(buf)) > 0) {
            out.write(buf, 0, len);
        }
        in.close();
        out.close();
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == 1 && resultCode == RESULT_OK && data != null) {
            selectedUri = data.getData();
            tvStatus.setText("Selected: " + selectedUri.toString());
            btnProtect.setEnabled(true);
        } else if (requestCode == 101) {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
                if (Environment.isExternalStorageManager()) {
                    Toast.makeText(this, "Permission Granted!", Toast.LENGTH_SHORT).show();
                } else {
                    Toast.makeText(this, "Permission Denied! Cannot save to SD card.", Toast.LENGTH_SHORT).show();
                }
            }
        }
    }

    public native boolean protectSo(String inputPath, String outputPath);
}
