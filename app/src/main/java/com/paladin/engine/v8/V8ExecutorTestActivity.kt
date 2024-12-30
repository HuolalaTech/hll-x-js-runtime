package com.paladin.engine.v8

import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.View
import android.widget.TextView
import com.paladin.engine.R
import com.paladin.jsruntime.common.JSRuntimeException
import com.paladin.jsruntime.v8.V8Executor

class V8ExecutorTestActivity : AppCompatActivity() {

    private lateinit var jsExecutor: V8Executor

    companion object {
        const val PARAM_FOR_EVALUATE = "paramForEvaluate"
    }


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_v8_executor_test)

        initV8Executor()

        initListener()
    }

    private fun initV8Executor() {
        try {
            jsExecutor = V8Executor(this)
        } catch (e: JSRuntimeException) {
            e.printStackTrace()
        }
    }

    override fun onDestroy() {
        super.onDestroy()
        jsExecutor.release()
    }

    private fun initListener() {
        findViewById<View>(R.id.evaluateVoidScript).setOnClickListener { // executeJS must be done in the background Thread, or it will ANR
            val intent = Intent(this, V8VoidScriptActivity::class.java)
            intent.putExtra(PARAM_FOR_EVALUATE, (it as TextView).text)
            startActivity(intent)
        }

        findViewById<View>(R.id.evaluateStringScript).setOnClickListener { // executeJS must be done in the background Thread.
            val intent = Intent(this, V8EvaluateStringScriptActivity::class.java)
            intent.putExtra(V8ExecutorTestActivity.PARAM_FOR_EVALUATE, (it as TextView).text)
            startActivity(intent)
        }

        findViewById<View>(R.id.callJavaScriptFunc).setOnClickListener {
            val intent = Intent(this, V8CallJavaScriptFuncTestActivity::class.java)
            intent.putExtra(V8ExecutorTestActivity.PARAM_FOR_EVALUATE, (it as TextView).text)
            startActivity(intent)
        }
        findViewById<View>(R.id.injectKVObject).setOnClickListener {
            val intent = Intent(this, V8CreateKVObjectTestActivity::class.java)
            intent.putExtra(V8ExecutorTestActivity.PARAM_FOR_EVALUATE, (it as TextView).text)
            startActivity(intent)
        }
        findViewById<View>(R.id.paladinMethodTest).setOnClickListener {
            val intent = Intent(this, V8PaladinMethodTestActivity::class.java)
            intent.putExtra(V8ExecutorTestActivity.PARAM_FOR_EVALUATE, (it as TextView).text)
            startActivity(intent)
        }

    }
}