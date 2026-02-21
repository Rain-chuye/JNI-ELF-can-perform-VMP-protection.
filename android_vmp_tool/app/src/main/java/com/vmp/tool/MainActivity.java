package com.vmp.tool;

import android.app.Activity;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Looper;
import android.provider.Settings;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.ScrollView;
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
    private TextView tvStatus, tvLog;
    private Button btnProtect;
    private ScrollView scrollLog;
    private ProgressBar progressBar;
    private Handler handler = new Handler(Looper.getMainLooper());

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        tvStatus = findViewById(R.id.tv_status);
        tvLog = findViewById(R.id.tv_log);
        btnProtect = findViewById(R.id.btn_protect);
        scrollLog = findViewById(R.id.scroll_log);
        progressBar = findViewById(R.id.progress_bar);
        Button btnSelect = findViewById(R.id.btn_select);

        checkPermissions();

        btnSelect.setOnClickListener(v -> {
            Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
            intent.setType("*/*");
            startActivityForResult(intent, 1);
        });

        btnProtect.setOnClickListener(v -> {
            if (selectedUri != null) {
                btnProtect.setEnabled(false);
                progressBar.setVisibility(View.VISIBLE);
                tvLog.setText("");
                new Thread(() -> processFile(selectedUri)).start();
            }
        });
    }

    // Called from JNI
    public void onLog(final String message) {
        handler.post(() -> {
            tvLog.append("> " + message + "\n");
            scrollLog.post(() -> scrollLog.fullScroll(View.FOCUS_DOWN));
        });
    }

    private void checkPermissions() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M && Build.VERSION.SDK_INT < Build.VERSION_CODES.R) {
            if (checkSelfPermission(android.Manifest.permission.READ_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
                requestPermissions(new String[]{android.Manifest.permission.READ_EXTERNAL_STORAGE, android.Manifest.permission.WRITE_EXTERNAL_STORAGE}, 100);
            }
        }
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            if (!Environment.isExternalStorageManager()) {
                try {
                    Intent intent = new Intent(Settings.ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION);
                    intent.setData(Uri.parse("package:" + getPackageName()));
                    startActivityForResult(intent, 101);
                } catch (Exception ignored) {}
            }
        }
    }

    private void processFile(Uri uri) {
        try {
            File cacheDir = getExternalCacheDir();
            if (cacheDir == null) cacheDir = getCacheDir();
            File tempIn = new File(cacheDir, "input.so");
            File tempOut = new File(cacheDir, "output.so");

            InputStream is = getContentResolver().openInputStream(uri);
            OutputStream os = new FileOutputStream(tempIn);
            byte[] buffer = new byte[8192];
            int read;
            while ((read = is.read(buffer)) != -1) os.write(buffer, 0, read);
            is.close(); os.close();

            boolean success = protectSo(tempIn.getAbsolutePath(), tempOut.getAbsolutePath());

            handler.post(() -> {
                progressBar.setVisibility(View.GONE);
                btnProtect.setEnabled(true);
                if (success) {
                    try {
                        File finalDir = new File(Environment.getExternalStorageDirectory(), "VMP");
                        if (!finalDir.exists()) finalDir.mkdirs();
                        File finalFile = new File(finalDir, "protected_" + System.currentTimeMillis() + ".so");
                        copyFile(tempOut, finalFile);
                        Toast.makeText(this, "Saved to: " + finalFile.getAbsolutePath(), Toast.LENGTH_LONG).show();
                    } catch (Exception e) {
                        onLog("Error saving: " + e.getMessage());
                    }
                } else {
                    Toast.makeText(this, "Protection Failed", Toast.LENGTH_SHORT).show();
                }
            });
        } catch (Exception e) {
            handler.post(() -> {
                progressBar.setVisibility(View.GONE);
                btnProtect.setEnabled(true);
                onLog("Exception: " + e.getMessage());
            });
        }
    }

    private void copyFile(File src, File dst) throws Exception {
        InputStream in = new FileInputStream(src);
        OutputStream out = new FileOutputStream(dst);
        byte[] buf = new byte[8192];
        int len;
        while ((len = in.read(buf)) > 0) out.write(buf, 0, len);
        in.close(); out.close();
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == 1 && resultCode == RESULT_OK && data != null) {
            selectedUri = data.getData();
            tvStatus.setText(selectedUri.getLastPathSegment());
            btnProtect.setEnabled(true);
        }
    }

    public native boolean protectSo(String inputPath, String outputPath);
}
