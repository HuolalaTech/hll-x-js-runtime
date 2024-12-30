package com.paladin.jsruntime.common.jsvalue;

public class JSArray extends JSValue {
    private final JSValue[] val;

    public JSArray(JSValue[] val) {
        this.val = val;
    }

    @Override
    public JSType getJSType() {
        return JSType.Array;
    }

    @Override
    public JSValue[] value() {
        return val;
    }
}
