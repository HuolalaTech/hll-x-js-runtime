package com.paladin.jsruntime.v8.inspector;

import android.util.Log;

import com.paladin.jsruntime.common.JSRuntimeException;
import com.paladin.jsruntime.v8.V8Executor;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.List;

public class V8Inspector {

    private final V8Executor v8Executor;
    private final long inspectorPtr;
    private boolean waitingForConnection = true;
    private final List<DebuggerConnectionListener> debuggerConnectionListeners;

    protected V8Inspector(final V8Executor v8Executor, final V8InspectorDelegate inspectorDelegate, final String contextName) {
        this.v8Executor = v8Executor;
        inspectorPtr = v8Executor.createInspector(inspectorDelegate, contextName);
        debuggerConnectionListeners = new ArrayList<>();
    }

    public static V8Inspector createV8Inspector(final V8Executor runtime, final V8InspectorDelegate inspectorDelegate, final String contextName) {
        return new V8Inspector(runtime, inspectorDelegate, contextName);
    }

    public static V8Inspector createV8Inspector(final V8Executor runtime, final V8InspectorDelegate inspectorDelegate) {
        return new V8Inspector(runtime, inspectorDelegate, "");
    }

    public void dispatchProtocolMessage(final String protocolMessage) {
        try {
            v8Executor.dispatchProtocolMessage(inspectorPtr, protocolMessage);
            if (waitingForConnection) {
                verifyDebuggerConnection(protocolMessage);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void addDebuggerConnectionListener(final DebuggerConnectionListener listener) {
        debuggerConnectionListeners.add(listener);
    }

    public void removeDebuggerConnectionListener(final DebuggerConnectionListener listener) {
        debuggerConnectionListeners.remove(listener);
    }

    private void verifyDebuggerConnection(final String protocolMessage) {
        try {
//            v8Executor.evaluateVoidScript("JSON.parse(JSON.stringify(" + protocolMessage + "))"); // TODO ？
            JSONObject json = new JSONObject(protocolMessage);
            if (json.getString("method").equals("Runtime.runIfWaitingForDebugger")) {
                Log.i("nyy", "protocolMessage's method = Runtime.runIfWaitingForDebugger");
                waitingForConnection = false;
                v8Executor.schedulePauseOnNextStatement(inspectorPtr, "");
                for (DebuggerConnectionListener listener : debuggerConnectionListeners) {
                    listener.onDebuggerConnected();
                }
            }
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

}
