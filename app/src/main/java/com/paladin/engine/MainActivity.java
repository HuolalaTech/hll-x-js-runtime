package com.paladin.engine;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;

import com.paladin.engine.jsc.JSCExecutorTestActivity;
import com.paladin.engine.qjs.QJSExecutorTestActivity;
import com.paladin.engine.debug.ExampleActivity;
import com.paladin.engine.v8.V8ExecutorTestActivity;

public class MainActivity extends AppCompatActivity {


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        findViewById(R.id.test_jsc).setOnClickListener(view -> {
            Intent intent = new Intent();
            intent.setClass(MainActivity.this, JSCExecutorTestActivity.class);
            startActivity(intent);
        });

        findViewById(R.id.test_qjs).setOnClickListener(view -> {
            Intent intent = new Intent();
            intent.setClass(MainActivity.this, QJSExecutorTestActivity.class);
            startActivity(intent);
        });

        findViewById(R.id.test_v8).setOnClickListener(view -> {
            Intent intent = new Intent();
            intent.setClass(MainActivity.this, V8ExecutorTestActivity.class);
            startActivity(intent);
        });
        findViewById(R.id.test_v8_debug).setOnClickListener(view -> {
            Intent intent = new Intent();
            intent.setClass(MainActivity.this, ExampleActivity.class);
            startActivity(intent);
        });
    }
}