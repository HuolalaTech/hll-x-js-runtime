package com.paladin.jsruntime.common.jsvalue;

public class JSString extends JSValue {

    private final String val;

    public JSString(String val) {
        this.val = val;
    }

    @Override
    public JSType getJSType() {
        return JSType.String;
    }

    @Override
    public String value() {
        return val;
    }
}
