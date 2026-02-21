package com.vmp.shield

import android.content.Intent
import android.net.Uri
import android.os.Build
import android.os.Bundle
import android.os.Environment
import android.provider.Settings
import android.widget.Button
import android.widget.TextView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import java.io.File

class MainActivity : AppCompatActivity() {

    private lateinit var statusText: TextView

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        statusText = findViewById(R.id.statusText)
        val btnProtect = findViewById<Button>(R.id.btnProtect)

        checkPermissions()

        btnProtect.setOnClickListener {
            runProtection()
        }
    }

    private fun checkPermissions() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            if (!Environment.isExternalStorageManager()) {
                val intent = Intent(Settings.ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION)
                intent.data = Uri.parse("package:\$packageName")
                startActivity(intent)
            }
        }
    }

    private fun runProtection() {
        val vmpDir = File(Environment.getExternalStorageDirectory(), "VMP")
        if (!vmpDir.exists()) vmpDir.mkdirs()

        statusText.text = "Status: Protecting..."

        // In a real app, we would use a file picker.
        // For this MVP, we assume a file exists or show the logic.

        Thread {
            try {
                // Call JNI to protect
                val result = protectFileNative("/sdcard/input.so", "/sdcard/VMP/protected.so")
                runOnUiThread {
                    statusText.text = "Status: Done! Saved to /sdcard/VMP/"
                    Toast.makeText(this, "Protection Complete", Toast.LENGTH_LONG).show()
                }
            } catch (e: Exception) {
                runOnUiThread { statusText.text = "Error: \${e.message}" }
            }
        }.start()
    }

    external fun protectFileNative(inputPath: String, outputPath: String): Int

    companion object {
        init {
            System.loadLibrary("vmp_shield")
        }
    }
}
