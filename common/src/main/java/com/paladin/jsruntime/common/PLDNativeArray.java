package com.paladin.jsruntime.common;


import com.paladin.jsruntime.common.jsvalue.JSObject;

/**
 * will be used by JNI
 */
public final class PLDNativeArray {

    // used by c++.
    @SuppressWarnings({"MismatchedReadAndWriteOfArray"})
    private final Object[] data;

    private int idx = 0;

    public PLDNativeArray(int size) {
        data = new Object[size];
    }

    public PLDNativeArray putInt(int data) {
        checkBounds();
        this.data[idx++] = data;
        return this;
    }

    public PLDNativeArray putBoolean(boolean data) {
        checkBounds();
        this.data[idx++] = data;
        return this;
    }

    public PLDNativeArray putDouble(double data) {
        checkBounds();
        this.data[idx++] = data;
        return this;
    }

    public PLDNativeArray putString(String data) {
        checkBounds();
        this.data[idx++] = data;
        return this;
    }

    public PLDNativeArray putJSObject(JSObject jsObject) {
        checkBounds();
        data[idx++] = jsObject;
        return this;
    }

    private void checkBounds() {
        if (idx >= data.length) {
            throw new ArrayIndexOutOfBoundsException();
        }
    }
}
