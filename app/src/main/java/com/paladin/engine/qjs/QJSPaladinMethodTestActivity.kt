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
import com.paladin.jsruntime.common.jsvalue.JSObject
import com.paladin.jsruntime.qjs.QJSExecutor


@SuppressLint("LongLogTag")
class QJSPaladinMethodTestActivity : AppCompatActivity() {

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

        testPaladinLog()

        testNativeRequire()

        testNativeBridge()

        testNativeSizeToFitList()

        testNativeSetTimeout()

        logE("========================================end========================================")
    }

    private fun testPaladinLog() {
        try {
            jsExecutor.evaluateVoidScript("paladinLog('test paladinLog function')", "")
        } catch (jsRuntimeException: JSRuntimeException) {
            jsRuntimeException.printStackTrace()
        }
    }

    private fun testNativeSetTimeout() {
        try {
            jsExecutor.evaluateVoidScript("nativeSetTimeout()", "")
        } catch (jsRuntimeException: JSRuntimeException) {
            jsRuntimeException.printStackTrace()
        }
    }

    private fun testNativeRequire() {
        try {
            jsExecutor.evaluateVoidScript("nativeRequire('@hll/paladin')", "")
        } catch (jsRuntimeException: JSRuntimeException) {
            jsRuntimeException.printStackTrace()
        }
    }

    private fun testNativeBridge() {
        try {
            jsExecutor.evaluateVoidScript("var ret = nativeBridge('host_01', 'vc module', 'needLayout', {'k1':'v1', 'k2':0}, 'callPCMethod'); paladinLog(ret[0] + ret[1]);", "")
        } catch (jsRuntimeException: JSRuntimeException) {
            jsRuntimeException.printStackTrace()
        }
    }

    private fun testNativeSizeToFitList() {
        val result: String?
        try {
            result =
                jsExecutor.evaluateStringScript("nativeSizeToFitList([{\"backgroundColor\":\"#40E0D0\",\"height\":0,\"hostId\":\"host_0\",\"needSizeToFit\":1,\"numberOfLines\":0,\"sizeKey\":\"Long long long long long sizeToFit text#0#20#none#false#false#0#150\",\"text\":\"Long long long long long sizeToFit text\",\"textColor\":\"#000000\",\"textSize\":20,\"type\":1,\"viewId\":\"viewid-5\",\"width\":150,\"x\":120,\"y\":372}])", "")
            Log.d(TAG, result!!)
        } catch (jsRuntimeException: JSRuntimeException) {
            jsRuntimeException.printStackTrace()
        }
    }

    private fun initJSExecutor() {
        jsExecutor.registerFunc("paladinLog", object : JavaFunction() {
            override fun exec(args: JSArray): PLDNativeArray? {
                if (args.value() != null) {
                    val jsValues = args.value()
                    if (jsValues != null && jsValues.isNotEmpty()) {
                        Log.d(TAG, "js call paladinLog: " + jsValues[0])
                    }
                }
                return null
            }
        })
        jsExecutor.registerFunc("nativeBridge", object : JavaFunction() {
            override fun exec(jsArray: JSArray): PLDNativeArray? {
                if (jsArray.value() != null) {
                    val jsValues = jsArray.value()
                    val hostId = jsValues[0].value() as String
                    val moduleName = jsValues[1].value() as String
                    val methodName = jsValues[2].value() as String
                    var args: JSObject? = null
                    if (jsArray.value().size > 3) {
                        args = jsValues[3] as JSObject
                    }
                    var callbackId = ""
                    if (jsArray.value().size > 4) {
                        callbackId = jsValues[4].value() as String
                    }
                    Log.d(
                        TAG,
                        "nativeBridge invoked, hostId: $hostId, moduleName: $moduleName, methodName: $methodName, args:$args, callbackId:$callbackId"
                    )
                    val pldNativeArray =
                        PLDNativeArray(2)
                    pldNativeArray.putDouble(2.0)
                    pldNativeArray.putString("hello")
                    val str = "ddd"
                    return pldNativeArray
                }
                return null
            }
        })

        jsExecutor.registerFunc("nativeSizeToFitList", object : JavaFunction() {
            override fun exec(jsArray: JSArray): PLDNativeArray? {
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