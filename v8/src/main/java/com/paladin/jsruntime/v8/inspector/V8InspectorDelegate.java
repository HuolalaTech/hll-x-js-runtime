package com.paladin.jsruntime.v8.inspector;

public interface V8InspectorDelegate {
    public void onResponse(String message);

    public void waitFrontendMessageOnPause();
}
