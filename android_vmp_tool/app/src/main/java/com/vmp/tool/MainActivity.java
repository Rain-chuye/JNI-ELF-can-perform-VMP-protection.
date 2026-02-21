package com.vmp.tool;

import android.app.Activity;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;
import java.io.File;

public class MainActivity extends Activity {

    static {
        System.loadLibrary("vmp_engine");
    }

    private String selectedPath = null;
    private TextView tvStatus;
    private Button btnProtect;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        tvStatus = findViewById(R.id.tv_status);
        btnProtect = findViewById(R.id.btn_protect);
        Button btnSelect = findViewById(R.id.btn_select);

        btnSelect.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // In a real app, use a file picker. For AIDE PoC, we might just use a hardcoded path
                // or a simple intent.
                Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
                intent.setType("*/*");
                startActivityForResult(intent, 1);
            }
        });

        btnProtect.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (selectedPath != null) {
                    String outPath = selectedPath.replace(".so", "_protected.so");
                    boolean success = protectSo(selectedPath, outPath);
                    if (success) {
                        Toast.makeText(MainActivity.this, "Protected file saved to: " + outPath, Toast.LENGTH_LONG).show();
                    } else {
                        Toast.makeText(MainActivity.this, "Protection failed", Toast.LENGTH_SHORT).show();
                    }
                }
            }
        });
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == 1 && resultCode == RESULT_OK && data != null) {
            Uri uri = data.getData();
            // Simplified path resolution (might need better handling for different URI types)
            selectedPath = uri.getPath();
            tvStatus.setText("Selected: " + selectedPath);
            btnProtect.setEnabled(true);
        }
    }

    public native boolean protectSo(String inputPath, String outputPath);
}
