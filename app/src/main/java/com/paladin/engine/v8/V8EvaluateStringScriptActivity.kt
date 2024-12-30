package com.paladin.engine.v8

import android.annotation.SuppressLint
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.widget.TextView
import com.paladin.engine.R
import com.paladin.jsruntime.common.JSRuntimeException
import com.paladin.jsruntime.common.JavaFunction
import com.paladin.jsruntime.common.PLDNativeArray
import com.paladin.jsruntime.common.jsvalue.JSArray
import com.paladin.jsruntime.v8.V8Executor
import javax.security.auth.login.LoginException

/**
 * TODO: 如何使用断言进行自动化测试？
 */
@SuppressLint("LongLogTag")
class V8EvaluateStringScriptActivity : AppCompatActivity() {

    private val jsExecutor by lazy { V8Executor(this) }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_evaluate_void_script)

        val testTitle = intent.getStringExtra(V8ExecutorTestActivity.PARAM_FOR_EVALUATE)
        findViewById<TextView>(R.id.evaluateTitle).text = testTitle

        initJSExecutor()

        // 测试正确的case，返回中英混合字符串
        val case0 = "function testReturnString() {return 'Hello Paladin, Welcome to 中国'}; testReturnString()" // logcat: Hello Paladin, Welcome to 中国

        // 测试返回number
        val case1 = "function testReturnOtherType() {return 1}; testReturnOtherType()" // logcat: 1

        logE("========================================begin========================================")

        logE("----------begin case 0----------")
        val result0: String? = jsExecutor.evaluateStringScript(case0, "")
        if (result0?.isNotEmpty() == true) logE(result0)
        logE("----------end case 0----------")

        logE("----------begin case 1----------")
        try {
            val result1: String? = jsExecutor.evaluateStringScript(case1, "")
            if (result1?.isNotEmpty() == true) logE(result1)

        } catch (e: JSRuntimeException) {
            e.printStackTrace()
        }
        logE("----------end case 1----------")

        logE("========================================end========================================")
    }

    private fun initJSExecutor() = try {
        jsExecutor.registerFunc("paladinLog", object : JavaFunction() {
            override fun exec(args: JSArray): PLDNativeArray? {
                if (args.value() != null) {
                    val jsValues = args.value()
                    if (jsValues != null && jsValues.isNotEmpty()) {
                        Log.i(TAG, "paladinLog exec: " + jsValues[0].toString())
                    }
                }
                return null
            }
        })
    } catch (jsRuntimeException: JSRuntimeException) {
        jsRuntimeException.printStackTrace()
    }

    private fun logE(log: String) = Log.e(TAG, log)

    override fun onDestroy() {
        super.onDestroy()
        jsExecutor.release()
    }

    companion object {
        const val TAG: String = "EvaluateStringScriptActivity"
    }
}