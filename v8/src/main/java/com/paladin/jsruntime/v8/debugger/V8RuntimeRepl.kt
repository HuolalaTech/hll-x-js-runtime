package com.paladin.jsruntime.v8.debugger

import android.util.Log
import com.facebook.stetho.inspector.console.RuntimeRepl
import com.paladin.jsruntime.v8.V8Executor
import java.util.Objects

class V8RuntimeRepl(private var v8Executor: V8Executor) : RuntimeRepl {
    override fun evaluate(p0: String?): Any? {
        Log.e("nyy", "$p0")
        p0?.let {
            return v8Executor.evaluateStringScript(it, "")
        }
        return Object()
    }
}