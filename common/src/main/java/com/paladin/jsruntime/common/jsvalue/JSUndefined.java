package com.paladin.jsruntime.common.jsvalue;

public class JSUndefined extends JSValue {

    @Override
    public JSType getJSType() {
        return JSType.UNDEFINED;
    }

    @Override
    public Object value() {
        return null;
    }
}
