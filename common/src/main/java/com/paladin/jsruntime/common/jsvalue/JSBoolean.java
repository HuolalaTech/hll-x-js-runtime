package com.paladin.jsruntime.common.jsvalue;

public class JSBoolean extends JSValue {

    private final boolean val;

    public JSBoolean(boolean val) {
        this.val = val;
    }

    @Override
    public JSType getJSType() {
        return JSType.Boolean;
    }

    @Override
    public Object value() {
        return val;
    }
}
