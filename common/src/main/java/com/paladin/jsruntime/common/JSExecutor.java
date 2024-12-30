package com.paladin.jsruntime.common;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.paladin.jsruntime.common.jsvalue.JSValue;

public abstract class JSExecutor {

    @SuppressWarnings("unused")
    private long mPtr; // used by native code

    public abstract void evaluateVoidScript(String script, String scriptUrl) throws JSRuntimeException;

    public abstract @Nullable String evaluateStringScript(String script, String scriptUrl) throws JSRuntimeException;

    public abstract JSValue callJavaScriptFunc(String thisObject, String methodId, PLDNativeArray args) throws JSRuntimeException;

    public abstract void registerFunc(String funcName, JavaFunction javaFunction) throws JSRuntimeException;

    public abstract void createKVObject(String jsObjName, String jsonValue) throws JSRuntimeException;

    public abstract void release();

    public abstract void protectJSObj(@NonNull String objName) throws JSRuntimeException;

    public abstract void unprotectJSObj(@NonNull String objName) throws JSRuntimeException;

    public abstract void gc();
}
