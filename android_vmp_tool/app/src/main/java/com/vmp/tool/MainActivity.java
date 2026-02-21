package com.vmp.tool;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import androidx.cardview.widget.CardView;

public class MainActivity extends Activity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        CardView cardSo = findViewById(R.id.card_so);
        CardView cardElf = findViewById(R.id.card_elf);

        cardSo.setOnClickListener(v -> {
            Intent intent = new Intent(this, SelectorActivity.class);
            intent.putExtra("mode", "so");
            startActivity(intent);
        });

        cardElf.setOnClickListener(v -> {
            Intent intent = new Intent(this, SelectorActivity.class);
            intent.putExtra("mode", "elf");
            startActivity(intent);
        });
    }
}
