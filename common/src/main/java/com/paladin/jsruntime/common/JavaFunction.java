package com.paladin.jsruntime.common;

import com.paladin.jsruntime.common.jsvalue.JSArray;

public abstract class JavaFunction {
    /**
     * @param jsArray 从JS侧传入的参数列表，按顺序存储在数组中
     */
    public abstract PLDNativeArray exec(JSArray jsArray);

    public boolean hashKey() {
        return false;
    }
}
