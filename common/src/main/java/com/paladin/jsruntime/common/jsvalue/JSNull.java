package com.paladin.jsruntime.common.jsvalue;

public class JSNull extends JSValue {

    @Override
    public JSType getJSType() {
        return JSType.Null;
    }

    @Override
    public Object value() {
        return null;
    }
}
