package com.paladin.engine.qjs

import android.content.Intent
import android.os.Bundle
import android.view.View
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import com.paladin.engine.R
import com.paladin.jsruntime.common.JSRuntimeException
import com.paladin.jsruntime.qjs.QJSExecutor

class QJSExecutorTestActivity : AppCompatActivity() {

    companion object {
        private const val TAG = "QJSExecutorTestActivity"
        const val PARAM_FOR_EVALUATE = "paramForEvaluate"
    }

    private lateinit var jsExecutor: QJSExecutor

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_qjsexecutor_test)

        initJSExecutor()

        initListener()
    }

    private fun initJSExecutor() {
        try {
            jsExecutor = QJSExecutor()
        } catch (e: JSRuntimeException) {
            e.printStackTrace()
        }
    }

    private fun initListener() {
        findViewById<View>(R.id.evaluateVoidScript).setOnClickListener { // executeJS must be done in the background Thread, or it will ANR
            val intent = Intent(this, QJSEvaluateVoidScriptActivity::class.java)
            intent.putExtra(PARAM_FOR_EVALUATE, (it as TextView).text)
            startActivity(intent)
        }
        findViewById<View>(R.id.evaluateStringScript).setOnClickListener { // executeJS must be done in the background Thread.
            val intent = Intent(this, QJSEvaluateStringScriptActivity::class.java)
            intent.putExtra(PARAM_FOR_EVALUATE, (it as TextView).text)
            startActivity(intent)
        }

        findViewById<View>(R.id.callJavaScriptFunc).setOnClickListener {
            val intent = Intent(this, QJSCallJavaScriptFuncTestActivity::class.java)
            intent.putExtra(PARAM_FOR_EVALUATE, (it as TextView).text)
            startActivity(intent)
        }
        findViewById<View>(R.id.injectKVObject).setOnClickListener {
            val intent = Intent(this, QJSCreateKVObjectTestActivity::class.java)
            intent.putExtra(PARAM_FOR_EVALUATE, (it as TextView).text)
            startActivity(intent)
        }
        findViewById<View>(R.id.paladinMethodTest).setOnClickListener {
            val intent = Intent(this, QJSPaladinMethodTestActivity::class.java)
            intent.putExtra(PARAM_FOR_EVALUATE, (it as TextView).text)
            startActivity(intent)
        }
    }

    override fun onDestroy() {
        super.onDestroy()
        jsExecutor.release()
    }

}