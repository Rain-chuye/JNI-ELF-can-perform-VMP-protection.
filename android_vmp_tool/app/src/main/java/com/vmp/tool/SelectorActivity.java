package com.vmp.tool;

import android.app.Activity;
import android.content.Intent;
import android.content.pm.ApplicationInfo;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Looper;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.List;

public class SelectorActivity extends Activity {
    static {
        System.loadLibrary("vmp_engine");
    }

    private String mode;
    private Uri selectedUri;
    private ListView lvFunctions;
    private List<String> symbols = new ArrayList<>();
    private List<Boolean> checked = new ArrayList<>();
    private Handler handler = new Handler(Looper.getMainLooper());
    private TextView tvLog;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_selector);

        mode = getIntent().getStringExtra("mode");
        ((TextView)findViewById(R.id.tv_title)).setText(mode.toUpperCase() + " Analyzer");

        lvFunctions = findViewById(R.id.lv_functions);
        Button btnSelect = findViewById(R.id.btn_select_file);
        Button btnExecute = findViewById(R.id.btn_execute);

        btnSelect.setOnClickListener(v -> {
            Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
            intent.setType("*/*");
            startActivityForResult(intent, 1);
        });

        btnExecute.setOnClickListener(v -> {
            if (selectedUri != null) {
                List<Integer> selectedIndices = new ArrayList<>();
                for (int i = 0; i < checked.size(); i++) {
                    if (checked.get(i)) selectedIndices.add(i);
                }
                int[] indices = new int[selectedIndices.size()];
                for (int i = 0; i < indices.length; i++) indices[i] = selectedIndices.get(i);

                new Thread(() -> processFile(selectedUri, indices)).start();
            }
        });
    }

    public void onLog(final String message) {
        handler.post(() -> {
            Toast.makeText(this, message, Toast.LENGTH_SHORT).show();
        });
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == 1 && resultCode == RESULT_OK && data != null) {
            selectedUri = data.getData();
            loadSymbols(selectedUri);
        }
    }

    private void loadSymbols(Uri uri) {
        try {
            File cacheDir = getCacheDir();
            File temp = new File(cacheDir, "scan.so");
            InputStream is = getContentResolver().openInputStream(uri);
            FileOutputStream os = new FileOutputStream(temp);
            byte[] buf = new byte[8192];
            int read;
            while ((read = is.read(buf)) != -1) os.write(buf, 0, read);
            is.close(); os.close();

            String[] syms = getElfSymbols(temp.getAbsolutePath());
            symbols.clear();
            checked.clear();
            if (syms != null) {
                for (String s : syms) {
                    symbols.add(s);
                    checked.add(false);
                }
            }

            ArrayAdapter<String> adapter = new ArrayAdapter<String>(this, R.layout.item_function, R.id.cb_func, symbols);
            lvFunctions.setAdapter(adapter);
            lvFunctions.setChoiceMode(ListView.CHOICE_MODE_MULTIPLE);
            lvFunctions.setOnItemClickListener((parent, view, position, id) -> checked.set(position, !checked.get(position)));

        } catch (Exception e) {
            Toast.makeText(this, "Failed: " + e.getMessage(), Toast.LENGTH_SHORT).show();
        }
    }

    private void processFile(Uri uri, int[] selectedIndices) {
        try {
            File cacheDir = getCacheDir();
            File tempIn = new File(cacheDir, "input.so");
            File tempOut = new File(cacheDir, "output.so");

            InputStream is = getContentResolver().openInputStream(uri);
            OutputStream os = new FileOutputStream(tempIn);
            byte[] buffer = new byte[8192];
            int read;
            while ((read = is.read(buffer)) != -1) os.write(buffer, 0, read);
            is.close(); os.close();

            ApplicationInfo ai = getPackageManager().getApplicationInfo(getPackageName(), 0);
            boolean success = protectAndPackSoWithSelection(tempIn.getAbsolutePath(), tempOut.getAbsolutePath(), ai.nativeLibraryDir, selectedIndices);

            if (success) {
                File finalDir = new File(Environment.getExternalStorageDirectory(), "VMP");
                if (!finalDir.exists()) finalDir.mkdirs();
                File finalFile = new File(finalDir, "protected_v14_" + System.currentTimeMillis() + ".so");
                copyFile(tempOut, finalFile);
                handler.post(() -> Toast.makeText(this, "Saved: " + finalFile.getAbsolutePath(), Toast.LENGTH_LONG).show());
            }
        } catch (Exception e) {
            handler.post(() -> Toast.makeText(this, "Error: " + e.getMessage(), Toast.LENGTH_LONG).show());
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

    public native String[] getElfSymbols(String path);
    public native boolean protectAndPackSoWithSelection(String inputPath, String outputPath, String libDir, int[] selectedIndices);
}
