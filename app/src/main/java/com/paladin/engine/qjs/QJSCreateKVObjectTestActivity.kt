package com.paladin.engine.qjs

import android.annotation.SuppressLint
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.widget.TextView
import android.widget.Toast
import com.paladin.engine.R
import com.paladin.jsruntime.common.JSRuntimeException
import com.paladin.jsruntime.qjs.QJSExecutor
import org.json.JSONObject

/**
 * TODO: 如何使用断言进行自动化测试？
 */
@SuppressLint("LongLogTag")
class QJSCreateKVObjectTestActivity : AppCompatActivity() {

    private val jsExecutor by lazy { QJSExecutor() }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_evaluate_void_script)

        val testTitle = intent.getStringExtra(QJSExecutorTestActivity.PARAM_FOR_EVALUATE)
        findViewById<TextView>(R.id.evaluateTitle).text = testTitle

        logE("========================================begin========================================")

        logE("-------------create KV Obj start---------------")
        val map = HashMap<String?, Any?>()
        map["key1"] = 1
        map["key2"] = 2.0f
        map["key3"] = 3.0
        map["key4"] = "val3"
        try {
            jsExecutor.createKVObject("LaBuLaDuo", JSONObject(map).toString())
        } catch (e: JSRuntimeException) {
            e.printStackTrace()
        }
        logE("-------------create KV Obj end---------------")


        logE("-------------test KV Obj start---------------")
        var result: String? = null
        try {
            result = jsExecutor.evaluateStringScript(
                "(function yy(){return 'key1: ' + LaBuLaDuo.key1 + ', key2: ' + LaBuLaDuo.key2 + ', key3: ' + LaBuLaDuo.key3 + ',  key4: ' + LaBuLaDuo.key4 })()",
            "")
        } catch (jsRuntimeException: JSRuntimeException) {
            jsRuntimeException.printStackTrace()
        }
        Toast.makeText(applicationContext, result ?: "null", Toast.LENGTH_LONG)
            .show()

        logE("-------------test KV Obj end---------------")
        logE("========================================end========================================")
    }

    private fun logE(log: String) = Log.e(TAG, log)

    override fun onDestroy() {
        super.onDestroy()
        jsExecutor.release()
    }

    companion object {
        const val TAG: String = "InjectKVObjectTestActivity"
    }
}