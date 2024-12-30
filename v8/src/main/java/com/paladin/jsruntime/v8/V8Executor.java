package com.paladin.jsruntime.v8;

import android.content.Context;
import android.content.res.AssetManager;
import android.os.Build;
import android.text.TextUtils;
import android.util.Log;

import androidx.annotation.NonNull;

import com.paladin.jsruntime.common.JSExecutor;
import com.paladin.jsruntime.common.JSRuntimeException;
import com.paladin.jsruntime.common.JavaFunction;
import com.paladin.jsruntime.common.PLDNativeArray;
import com.paladin.jsruntime.common.jsvalue.JSValue;
import com.paladin.jsruntime.v8.debugger.StethoHelper;
import com.paladin.jsruntime.v8.debugger.V8Debugger;
import com.paladin.jsruntime.v8.inspector.V8InspectorDelegate;

public class V8Executor extends JSExecutor {

    private static final String TAG = "V8Executor";
    private static AssetManager am; //used by native code
    private final long mPtr; // used by native code

    static {
        System.loadLibrary("v8_runtime");
        System.loadLibrary("v8android");
    }

    // called by native code
    public static String loadDefaultSnapshotBlobPath() {
        String assetsPath;
        if (Build.VERSION.SDK_INT < 21) {
            assetsPath = Build.CPU_ABI + "/snapshot_blob.bin";
        } else {
            assetsPath = Build.SUPPORTED_ABIS[0] + "/snapshot_blob.bin";
        }
        Log.i(TAG, assetsPath);
        return assetsPath;
    }

    public V8Executor(@NonNull Context context) throws JSRuntimeException {
        am = context.getApplicationContext().getAssets();
        mPtr = nativeInit();
    }

    @Override
    public void evaluateVoidScript(String script, String scriptUrl) throws JSRuntimeException {
        if (!TextUtils.isEmpty(scriptUrl) && !TextUtils.isEmpty(script)) {
            StethoHelper.INSTANCE.getScriptSourceProvider().put(scriptUrl, script);
        }
        nativeEvaluateVoidScript(mPtr, script, scriptUrl);
    }

    @Override
    public String evaluateStringScript(String script, String scriptUrl) throws JSRuntimeException {
        if (!TextUtils.isEmpty(scriptUrl) && !TextUtils.isEmpty(script)) {
            StethoHelper.INSTANCE.getScriptSourceProvider().put(scriptUrl, script);
        }
        return nativeEvaluateStringScript(mPtr, script, scriptUrl);
    }

    @Override
    public JSValue callJavaScriptFunc(String thisObject, String methodId, PLDNativeArray args) throws JSRuntimeException {
        return nativeCallJavaScriptFunc(mPtr, thisObject, methodId, args);
    }

    @Override
    public void registerFunc(String funcName, JavaFunction javaFunction) throws JSRuntimeException {
        nativeRegisterFunc(mPtr, funcName, javaFunction);
    }

    @Override
    public void createKVObject(String jsObjName, String jsonValue) throws JSRuntimeException {
        String str = "var " + jsObjName + " = " + jsonValue;
        nativeEvaluateVoidScript(mPtr, str, jsObjName);
    }

    @Override
    public void release() {
        nativeRelease(mPtr);
    }

    @Override
    public void protectJSObj(@NonNull String objName) {

    }

    @Override
    public void unprotectJSObj(@NonNull String objName) {

    }

    public long createInspector(final V8InspectorDelegate inspectorDelegate, final String contextName) {
        return nativeCreateInspector(mPtr, inspectorDelegate, contextName);
    }

    public void dispatchProtocolMessage(final long V8InspectorPtr, final String protocolMessage) {
        nativeDispatchProtocolMessage(mPtr, V8InspectorPtr, protocolMessage);
    }

    public void schedulePauseOnNextStatement(final long V8InspectorPtr, final String reason) {
        nativeSchedulePauseOnNextStatement(mPtr, V8InspectorPtr, reason);
    }

    @Override
    public void gc() {

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

    private static native long nativeCreateInspector(final long ptr, final V8InspectorDelegate inspectorDelegate, final String contextName);

    private static native void nativeDispatchProtocolMessage(final long ptr, final long inspectorPtr, String protocolMessage);

    private static native void nativeSchedulePauseOnNextStatement(final long ptr, long v8InspectorPtr, final String reason);

}
