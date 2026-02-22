package com.vmp.tool;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ApplicationInfo;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Looper;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;
import com.google.android.material.checkbox.MaterialCheckBox;
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
    private FunctionAdapter adapter;
    private List<String> symbols = new ArrayList<>();
    private List<Boolean> checkedState = new ArrayList<>();
    private Handler handler = new Handler(Looper.getMainLooper());
    private MaterialCheckBox cbSelectAll;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_selector);

        mode = getIntent().getStringExtra("mode");
        ((TextView)findViewById(R.id.tv_title)).setText(mode.toUpperCase() + " Analyzer");

        lvFunctions = findViewById(R.id.lv_functions);
        cbSelectAll = findViewById(R.id.cb_select_all);
        Button btnSelect = findViewById(R.id.btn_select_file);
        Button btnExecute = findViewById(R.id.btn_execute);

        adapter = new FunctionAdapter(this);
        lvFunctions.setAdapter(adapter);

        btnSelect.setOnClickListener(v -> {
            Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
            intent.setType("*/*");
            startActivityForResult(intent, 1);
        });

        cbSelectAll.setOnCheckedChangeListener((buttonView, isChecked) -> {
            for (int i = 0; i < checkedState.size(); i++) {
                checkedState.set(i, isChecked);
            }
            adapter.notifyDataSetChanged();
        });

        lvFunctions.setOnItemClickListener((parent, view, position, id) -> {
            checkedState.set(position, !checkedState.get(position));
            adapter.notifyDataSetChanged();
        });

        btnExecute.setOnClickListener(v -> {
            if (selectedUri != null) {
                List<Integer> selectedIndices = new ArrayList<>();
                for (int i = 0; i < checkedState.size(); i++) {
                    if (checkedState.get(i)) selectedIndices.add(i);
                }
                int[] indices = new int[selectedIndices.size()];
                for (int i = 0; i < indices.length; i++) indices[i] = selectedIndices.get(i);

                new Thread(() -> processFile(selectedUri, indices)).start();
            }
        });
    }

    private class FunctionAdapter extends BaseAdapter {
        private LayoutInflater inflater;

        public FunctionAdapter(Context context) {
            inflater = LayoutInflater.from(context);
        }

        @Override
        public int getCount() { return symbols.size(); }
        @Override
        public Object getItem(int position) { return symbols.get(position); }
        @Override
        public long getItemId(int position) { return position; }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            if (convertView == null) {
                convertView = inflater.inflate(R.layout.item_function, parent, false);
            }
            TextView tvName = convertView.findViewById(R.id.tv_name);
            MaterialCheckBox cb = convertView.findViewById(R.id.cb_item);

            tvName.setText(symbols.get(position));
            cb.setChecked(checkedState.get(position));

            return convertView;
        }
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
            checkedState.clear();
            if (syms != null) {
                for (String s : syms) {
                    symbols.add(s);
                    checkedState.add(false);
                }
            }
            cbSelectAll.setChecked(false);
            adapter.notifyDataSetChanged();

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
