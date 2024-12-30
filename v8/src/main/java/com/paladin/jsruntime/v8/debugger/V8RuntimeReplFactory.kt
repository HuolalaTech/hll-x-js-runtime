package com.paladin.jsruntime.v8.debugger

import com.facebook.stetho.inspector.console.RuntimeRepl
import com.facebook.stetho.inspector.console.RuntimeReplFactory
import com.paladin.jsruntime.v8.V8Executor

class V8RuntimeReplFactory(private var v8Executor: V8Executor) : RuntimeReplFactory{
    override fun newInstance(): RuntimeRepl {
        return V8RuntimeRepl(v8Executor)
    }
}