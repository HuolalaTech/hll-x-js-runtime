package com.paladin.engine.qjs

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
import com.paladin.jsruntime.qjs.QJSExecutor

/**
 * TODO: 如何使用断言进行自动化测试？
 */
class QJSEvaluateVoidScriptActivity : AppCompatActivity() {

    private val jsExecutor by lazy { QJSExecutor() }

    @SuppressLint("LongLogTag")
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_evaluate_void_script)

        val testTitle = intent.getStringExtra(QJSExecutorTestActivity.PARAM_FOR_EVALUATE)
        findViewById<TextView>(R.id.evaluateTitle).text = testTitle

        initJSExecutor()

        // 定义变量，作为一个js方法的入参
        val case0 = "var i = 0; paladinLog(i)" // logcat: paladinLog exec: 0.0

        // 调用不存在的js方法
        val case1 = "console.log('Paladin is not just a name, it is a story')" // logcat: ReferenceError: 'console' is not defined

        // 测试An IIFE (Immediately Invoked Function Expression)
        val case2 = "(function paladin() { paladinLog('Hello Paladin')})()" // logcat: paladinLog exec: Hello Paladin

        // 测试ICU
        val case3 = "var nf = new Intl.NumberFormat(undefined, {style:'currency', currency:'GBP'}); nf.format(100);" //logcat: ReferenceError: 'Intl' is not defined

        // 定义一个函数，并调用它
        val case4 = "function test(a1, a2, a3, a4) {return a1 + a2 + a3 + a4;}; paladinLog(test(1, 2.0, 3, 'hell'))" // logcat: paladinLog exec: 6hell

        // 测试中文
        val case5 = "var i = '取一杯天上的水'; paladinLog(i)" // logcat: paladinLog exec: 取一杯天上的水

        logE("========================================begin========================================")

        logE("----------begin case 0----------")
        jsExecutor.evaluateVoidScript(case0, "")
        logE("----------end case 0----------")

        logE("----------begin case 1----------")
        try {
            jsExecutor.evaluateVoidScript(case1, "")
        } catch (e: JSRuntimeException) {
            e.printStackTrace()
        }
        logE("----------end case 1----------")

        logE("----------begin case 2----------")
        jsExecutor.evaluateVoidScript(case2, "")
        logE("----------end case 2----------")

        logE("----------begin case 3----------")
        try {
            jsExecutor.evaluateVoidScript(case3, "")
        } catch (e: JSRuntimeException) {
            e.printStackTrace()
        }
        logE("----------end case 3----------")

        logE("----------begin case 4----------")
        jsExecutor.evaluateVoidScript(case4, "")
        logE("----------end case 4----------")

        logE("----------begin case 5----------")
        jsExecutor.evaluateVoidScript(case5, "")
        logE("----------end case 5----------")

        logE("========================================end========================================")
    }

    private fun initJSExecutor() = try {
        jsExecutor.registerFunc("paladinLog", object : JavaFunction() {
            @SuppressLint("LongLogTag")
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

    @SuppressLint("LongLogTag")
    private fun logE(log: String) = Log.e(TAG, log)

    override fun onDestroy() {
        super.onDestroy()
        jsExecutor.release()
    }

    companion object {
        const val TAG: String = "EvaluateVoidScriptActivity"
    }
}