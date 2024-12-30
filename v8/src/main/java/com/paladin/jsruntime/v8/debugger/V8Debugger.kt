package com.paladin.jsruntime.v8.debugger

import android.content.Context
import android.os.Handler
import android.os.HandlerThread
import android.os.Looper
import com.paladin.jsruntime.v8.V8Executor
import org.json.JSONObject


object V8Debugger {
    private const val MAX_SCRIPT_CACHE_SIZE = 10000000
    private const val MAX_DEPTH = 32

    /**
     * Utility, which simplifies configuring V8 for debugging support and creation of new instance.
     * Creates V8 runtime, v8 debugger and binds it to Stetho.
     *
     * @param v8Executor single-thread executor where v8 will be created
     *  and all debug calls will be performed by Stetho later.
     *
     * NOTE: Should be declared as V8 class extensions when will be allowed (https://youtrack.jetbrains.com/issue/KT-11968)
     */
    fun init(v8Executor: V8Executor, handlerThread: HandlerThread) {
        val runnable = Runnable {
            val messenger = V8Messenger(v8Executor)
            with(messenger) {
                // Default Chrome DevTool protocol messages
                sendMessage(Protocol.Runtime.Enable)

                sendMessage(Protocol.Debugger.Enable, JSONObject().put("maxScriptsCacheSize", MAX_SCRIPT_CACHE_SIZE))
                sendMessage(Protocol.Debugger.SetAsyncCallStackDepth, JSONObject().put("maxDepth", MAX_DEPTH))
                sendMessage(Protocol.Runtime.RunIfWaitingForDebugger)
            }

            StethoHelper.initializeWithV8Messenger(messenger, handlerThread)
        }
        if (Looper.myLooper() != handlerThread.looper) {
            val handler = Handler(handlerThread.looper)
            handler.post(runnable)
        } else {
            runnable.run()
        }
    }
}
