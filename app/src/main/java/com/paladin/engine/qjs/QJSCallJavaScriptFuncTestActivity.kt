package com.paladin.engine.qjs

import android.annotation.SuppressLint
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.widget.TextView
import android.widget.Toast
import com.paladin.engine.R
import com.paladin.jsruntime.common.JSRuntimeException
import com.paladin.jsruntime.common.JavaFunction
import com.paladin.jsruntime.common.PLDNativeArray
import com.paladin.jsruntime.common.jsvalue.JSArray
import com.paladin.jsruntime.common.jsvalue.JSObject
import com.paladin.jsruntime.common.jsvalue.JSValue
import com.paladin.jsruntime.qjs.QJSExecutor
import org.json.JSONObject


@SuppressLint("LongLogTag")
class QJSCallJavaScriptFuncTestActivity : AppCompatActivity() {

    companion object {
        private const val TAG = "CallJavaScriptFuncTestActivity"
        private const val testJSON = "{\"#1#16#none#false#false#0\":{\"width\":0,\"height\":19},\"个人信息#1#16#none#false#false#0\":{\"width\":64,\"height\":19},\"账号#1#16#none#false#false#0\":{\"width\":32,\"height\":19},\"去添加#1#16#none#false#false#0\":{\"width\":48,\"height\":19},\"紧急联系人#1#16#none#false#false#0\":{\"width\":80,\"height\":19},\"隐私权限管理#1#16#none#false#false#0\":{\"width\":96,\"height\":19},\"给货拉拉好评#1#16#none#false#false#0\":{\"width\":96,\"height\":19},\"分享#1#16#none#false#false#0\":{\"width\":32,\"height\":19},\"收费标准#1#16#none#false#false#0\":{\"width\":64,\"height\":19},\"货拉拉法律条款#1#16#none#false#false#0\":{\"width\":112,\"height\":19},\"v6.6.4#1#16#none#false#false#0\":{\"width\":44,\"height\":19},\"关于货拉拉#1#16#none#false#false#0\":{\"width\":80,\"height\":19}}"
    }

    private val jsExecutor by lazy { QJSExecutor() }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_evaluate_void_script)

        val testTitle = intent.getStringExtra(QJSExecutorTestActivity.PARAM_FOR_EVALUATE)
        findViewById<TextView>(R.id.evaluateTitle).text = testTitle

        initJSExecutor()


        logE("========================================begin========================================")
        // case0 成功执行test函数，并Toast成功==================================
        logE("----------begin case 0----------")
        val args = PLDNativeArray(5)
        args.putInt(1);
        args.putInt(2)
        args.putDouble(3.0)
        args.putString("Hell")
        val jsonObject = JSONObject(testJSON)
        args.putJSObject(JSObject(jsonObject.toString()))
        try {
            val jsValue : JSValue? = jsExecutor.callJavaScriptFunc("", "test", args)
            jsValue?.let {
                Toast.makeText(
                    applicationContext,
                    jsValue.toString(),
                    Toast.LENGTH_LONG
                ).show()
            }
        } catch (e: JSRuntimeException) {
            e.printStackTrace()
        }
        logE("----------end case 0----------")


        // case1 执行一个不存在的JS方法，报错JS Exception: TypeError: not a function
        logE("----------begin case 1----------")
        try {
            jsExecutor.callJavaScriptFunc("", "test1111111", args)
        } catch (e: JSRuntimeException) {
            e.printStackTrace()
        }
        logE("----------end case 1----------")

        // case2 执行一个不是GlobalObject对象上的方法
        logE("----------begin case 2----------")
        jsExecutor.evaluateVoidScript("var Cat = {" +
                "  type: 'Invertebrates', " +
                "  displayType : function() { " +
                "    paladinLog(this.type);" +
                "  }" +
                "}", "")
        // This must be careful. Java has Checked Exceptions, but Kotlin has no Checked Exceptions
        // https://kotlinlang.org/docs/exceptions.html#the-nothing-type
        jsExecutor.callJavaScriptFunc("Cat", "displayType", null) // logcat: Invertebrates
        logE("----------end case 2----------")

        // case3 执行一个不是GlobalObject对象上未定义的方法
        logE("----------begin case 3----------")
        try {
            jsExecutor.callJavaScriptFunc("Animal", "displayType", null) // logcat: TypeError: not a function
        } catch (e: JSRuntimeException) {
            e.printStackTrace()
        }
        logE("----------end case 3----------")

        logE("========================================end========================================")
    }

    private fun initJSExecutor() {
        jsExecutor.registerFunc("test", object : JavaFunction() {
            override fun exec(args: JSArray): PLDNativeArray? {
                if (args.value() != null) {
                    val jsValues = args.value()
                    if (jsValues != null && jsValues.isNotEmpty()) {
                        var ret = ""
                        for (i in jsValues.indices) {
                            Log.d(TAG, i.toString() + ":" + jsValues[i])
                            ret += jsValues[i].toString()
                        }
                        Log.i(TAG, ret)
                        val pldNativeArray =
                            PLDNativeArray(3)
                        pldNativeArray.putDouble(15.0)
                        pldNativeArray.putInt(-1)
                        pldNativeArray.putString(ret)
                        return pldNativeArray
                    }
                }
                return null
            }
        })

        jsExecutor.registerFunc("paladinLog", object : JavaFunction() {
            override fun exec(args: JSArray): PLDNativeArray? {
                if (args.value() != null) {
                    val jsValues = args.value()
                    if (jsValues != null && jsValues.isNotEmpty()) {
                        logE(jsValues[0].asString().value())
                    }
                }
                return null
            }
        })
    }

    private fun logE(log: String) = Log.e(TAG, log)

    override fun onDestroy() {
        super.onDestroy()
        jsExecutor.release()
    }
}