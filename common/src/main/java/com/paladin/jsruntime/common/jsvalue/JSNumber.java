package com.paladin.jsruntime.common.jsvalue;

public class JSNumber extends JSValue {

    private final double val;

    public JSNumber(double val) {
        this.val = val;
    }

    @Override
    public JSType getJSType() {
        return JSType.Number;
    }

    @Override
    public Double value() {
        return val;
    }
}
