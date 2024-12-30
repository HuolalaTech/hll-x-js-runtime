package com.paladin.jsruntime.v8.inspector;

public interface DebuggerConnectionListener {
    public void onDebuggerConnected();

    public void onDebuggerDisconnected();
}
