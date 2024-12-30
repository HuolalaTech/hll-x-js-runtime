package com.paladin.jsruntime.jsc;

import android.text.TextUtils;

import androidx.annotation.NonNull;

import com.paladin.jsruntime.common.JSExecutor;
import com.paladin.jsruntime.common.JSRuntimeException;
import com.paladin.jsruntime.common.JavaFunction;
import com.paladin.jsruntime.common.PLDNativeArray;
import com.paladin.jsruntime.common.jsvalue.JSValue;

public class JSCExecutor extends JSExecutor {

    private final long mPtr; // used by native code

    static {
        System.loadLibrary("jsc_runtime");
        System.loadLibrary("jsc");
    }

    public JSCExecutor() throws JSRuntimeException {
        mPtr = nativeInit();
    }

    public void evaluateVoidScript(String script, String scriptUrl)  throws JSRuntimeException  {
        nativeEvaluateVoidScript(mPtr, script, scriptUrl);
    }

    public String evaluateStringScript(String script, String scriptUrl)  throws JSRuntimeException  {
        return nativeEvaluateStringScript(mPtr, script, scriptUrl);
    }

    public JSValue callJavaScriptFunc(String thisObject, String methodId, PLDNativeArray args)  throws JSRuntimeException {
        return nativeCallJavaScriptFunc(mPtr, thisObject, methodId, args);
    }

    public void registerFunc(String funcName, JavaFunction javaFunction) {
        nativeRegisterFunc(mPtr, funcName, javaFunction);
    }

    public void createKVObject(String jsObjName, String jsonValue) throws JSRuntimeException {
        String str = "var " + jsObjName + " = " + jsonValue;
        nativeEvaluateVoidScript(mPtr, str, jsObjName);
    }

    public void release() {
        nativeRelease(mPtr);
    }

    public void protectJSObj(@NonNull String objName) {
        if (TextUtils.isEmpty(objName)) return;
        nativeProtectJSObj(mPtr, objName);
    }

    public void unprotectJSObj(@NonNull String objName) {
        if (TextUtils.isEmpty(objName)) return;
        nativeUnprotectJSObj(mPtr, objName);
    }

    public void gc() {
        nativeGc(mPtr);
    }

    private static native long nativeInit() throws JSRuntimeException;

    private static native void nativeRelease(long ptr);

    private static native void nativeEvaluateVoidScript(long ptr, String script, String scriptUrl) throws JSRuntimeException;

    private static native String nativeEvaluateStringScript(long ptr, String script, String scriptUrl) throws JSRuntimeException;

    private static native JSValue nativeCallJavaScriptFunc(long ptr, String thisObj, String methodId, PLDNativeArray pldNativeArray) throws JSRuntimeException;

    private static native void nativeRegisterFunc(long ptr, String funcName, JavaFunction javaFunction);

    private static native void nativeProtectJSObj(long ptr, @NonNull String objName);

    private static native void nativeUnprotectJSObj(long ptr, @NonNull String objName);

    private static native void nativeGc(long ptr);

}
