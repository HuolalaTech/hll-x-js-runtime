package com.paladin.engine.debug

import android.os.Bundle
import android.os.Handler
import android.os.HandlerThread
import android.widget.Button
import androidx.appcompat.app.AppCompatActivity
import com.google.android.material.snackbar.Snackbar
import com.paladin.engine.R
import com.paladin.jsruntime.v8.V8Executor
import com.paladin.jsruntime.v8.debugger.V8Debugger
import java.text.DateFormat
import java.util.*

class ExampleActivity : AppCompatActivity() {

    private val handlerThread: HandlerThread = HandlerThread("paladin-runtime-debugger")
    private var handler: Handler? = null
    private var v8Executor: V8Executor? = null

    override fun onCreate(savedInstanceState: Bundle?) {

        handlerThread.start()
        handler = Handler(handlerThread.looper)

        handler?.post {
            v8Executor = V8Executor(applicationContext)
            V8Debugger.init(v8Executor!!, handlerThread)
        }

        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_example)

        findViewById<Button>(R.id.fab).setOnClickListener { view ->
            val scriptUrl = "hello-world0.js"
            val date = DateFormat.getTimeInstance().format(Date())
            val script = ("""
                |var globalHi = "hi from $scriptUrl"
                |
                |function main(payloadObject) {
                |  var hello = 'hello, ';
                |  var world = 'world';
                |
                |  var testReload = '$date';
                |
                |  return globalHi + ' and ' + hello + world + ' at ' + testReload + ' with ' + payloadObject.load + ' !';
                |}
                |
                |main({
                |    load: 'object based payload',
                |    redundantLoad: 'this is ignored',
                |    callBack: function testCallBack() { print('Call back!') }
                |})
            """).trimMargin()

            handler?.post {
                val result = v8Executor?.evaluateStringScript(script, scriptUrl)
                println("[v8 execution result: ] $result")

                Snackbar.make(view, "V8 answers: $result", Snackbar.LENGTH_SHORT)
                    .setAction("V8Action", null).show()
            }
        }
    }

    override fun onDestroy() {
        releaseDebuggableV8()

        super.onDestroy()
    }

    private fun releaseDebuggableV8() {
        handler?.post {
            v8Executor?.release()
        }
    }
}
