package com.paladin.jsruntime.common.jsvalue;

public abstract class JSValue {
    public enum JSType{
        UNDEFINED,
        Null,
        Number,
        Boolean,
        String,
        Object,
        Array,
    }

    public abstract JSType getJSType();

    public abstract Object value();

    public boolean isUndefined() {
        return getJSType() == JSType.UNDEFINED;
    }

    public boolean isNull() {
        return getJSType() == JSType.Null;
    }

    public boolean isNumber() {
        return getJSType() == JSType.Number;
    }

    public boolean isBoolean() {
        return getJSType() == JSType.Boolean;
    }

    public boolean isString() {
        return getJSType() == JSType.String;
    }

    public boolean isObject() {
        return getJSType() == JSType.Object;
    }

    public boolean isArray() {
        return getJSType() == JSType.Array;
    }

    public JSUndefined asUndefined() {
        return (JSUndefined) this;
    }

    public JSNull asNull() {
        return (JSNull) this;
    }

    public JSNumber asNumber() {
        return (JSNumber) this;
    }

    public JSBoolean asBoolean() {
        return (JSBoolean) this;
    }

    public JSString asString() {
        return (JSString) this;
    }

    public JSObject asObject() {
        return (JSObject) this;
    }

    public JSArray asArray() {
        return (JSArray) this;
    }

    @Override
    public String toString() {
        return String.valueOf(value());
    }
}
