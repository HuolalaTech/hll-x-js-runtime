package com.paladin.jsruntime.common.jsvalue;

public class JSObject extends JSValue {

    // 暂时使用json str, 例如：{"k1":"v1", "k2":2.0}
    private final String val;

    public JSObject(String val) {
        this.val = val;
    }


    @Override
    public JSType getJSType() {
        return JSType.Object;
    }

    @Override
    public String value() {
        return val;
    }
}
