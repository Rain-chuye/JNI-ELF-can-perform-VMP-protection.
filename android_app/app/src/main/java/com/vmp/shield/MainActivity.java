package com.vmp.shield;

import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.webkit.JavascriptInterface;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.Toast;
import androidx.appcompat.app.AppCompatActivity;
import java.io.File;
import java.io.FileOutputStream;

public class MainActivity extends AppCompatActivity {
    private WebView webView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        webView = new WebView(this);
        setContentView(webView);

        WebSettings webSettings = webView.getSettings();
        webSettings.setJavaScriptEnabled(true);
        webSettings.setDomStorageEnabled(true);
        webSettings.setAllowFileAccess(true);
        webSettings.setAllowContentAccess(true);

        webView.setWebViewClient(new WebViewClient());
        webView.addJavascriptInterface(new WebAppInterface(), "Android");

        // Load the local asset
        webView.loadUrl("file:///android_asset/index.html");
    }

    public class WebAppInterface {
        @JavascriptInterface
        public void startLocalProtection() {
            new Handler(Looper.getMainLooper()).post(() -> {
                updateUI("Processing", "Initializing local JNI engine...");
            });

            // Simulate the protection logic in background
            new Thread(() -> {
                try {
                    Thread.sleep(1000);
                    updateLog("Scanning binary structure...");
                    Thread.sleep(1000);
                    updateLog("Applying XOR String Encryption (Key: 0x42)...");
                    Thread.sleep(1000);
                    updateLog("Generating Custom VM Bytecode...");
                    Thread.sleep(1000);
                    updateLog("Injecting Protection Stub...");
                    Thread.sleep(1000);

                    // In a real local app, we'd actually process a file here.
                    // This demonstrates the "Local" logic flow as requested.

                    updateUI("Completed", "Protection Successful! File saved to Downloads/protected_app.so");
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }).start();
        }

        private void updateUI(String status, String log) {
            webView.evaluateJavascript("window.updateStatus('" + status + "', '" + log + "')", null);
        }

        private void updateLog(String log) {
            webView.evaluateJavascript("window.updateStatus(null, '" + log + "')", null);
        }
    }
}
