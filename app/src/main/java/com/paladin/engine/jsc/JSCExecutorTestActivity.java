package com.paladin.engine.jsc;

import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import com.paladin.engine.R;
import com.paladin.jsruntime.common.JSExecutor;
import com.paladin.jsruntime.common.JSRuntimeException;
import com.paladin.jsruntime.common.JavaFunction;
import com.paladin.jsruntime.common.PLDNativeArray;
import com.paladin.jsruntime.common.jsvalue.JSArray;
import com.paladin.jsruntime.common.jsvalue.JSObject;
import com.paladin.jsruntime.common.jsvalue.JSValue;
import com.paladin.jsruntime.jsc.JSCExecutor;

import org.json.JSONObject;

import java.util.HashMap;

public class JSCExecutorTestActivity extends AppCompatActivity {

    private static final String TAG = "JSExecutorTestActivity";

    private final Float padding = 20F;

    private JSExecutor jsExecutor;

    public JSExecutor getJSExecutor() {
        return jsExecutor;
    }

    private void initJSExecutor() {
        try {
            jsExecutor = new JSCExecutor();
            jsExecutor.registerFunc("paladinLog", new JavaFunction() {
                @Override
                public PLDNativeArray exec(JSArray args) {
                    if (args != null && args.value() != null) {
                        JSValue[] jsValues = args.value();
                        if (jsValues != null && jsValues.length > 0) {
                            Log.d(TAG, "js call paladinLog: " + jsValues[0]);
                        }
                    }
                    return null;
                }
            });

            jsExecutor.registerFunc("nativeBridge", new JavaFunction() {
                @Override
                public PLDNativeArray exec(JSArray jsArray) {
                    if (jsArray!= null && jsArray.value() != null) {
                        JSValue[] jsValues = jsArray.value();
                        String hostId = (String) jsValues[0].value();
                        String moduleName = (String) jsValues[1].value();
                        String methodName = (String) jsValues[2].value();
                        JSObject args = null;
                        if (jsArray.value().length > 3) {
                            args = (JSObject) jsValues[3];
                        }
                        String callbackId = "";
                        if (jsArray.value().length > 4) {
                            callbackId = (String) jsValues[4].value();
                        }
                        Log.d(TAG, "nativeBridge invoked, hostId: " + hostId + ", moduleName: " + moduleName + ", methodName: " + methodName + ", args:" + args + ", callbackId:" + callbackId);
                        PLDNativeArray pldNativeArray = new PLDNativeArray(2);
                        pldNativeArray.putDouble(2.0f);
                        pldNativeArray.putString("hello");
                        return pldNativeArray;
                    }
                    return null;
                }
            });

            jsExecutor.registerFunc("test", new JavaFunction() {
                @Override
                public PLDNativeArray exec(JSArray args) {
                    if (args != null && args.value() != null) {
                        JSValue[] jsValues = args.value();
                        if (jsValues != null && jsValues.length > 0) {
                            for (int i = 0; i < jsValues.length; ++i) {
                                Log.d(TAG, i + ":" + jsValues[i]);
                            }
                        }
                    }
                    return null;
                }
            });

            jsExecutor.registerFunc("nativeSizeToFitList", new JavaFunction() {
                @Override
                public PLDNativeArray exec(JSArray jsArray) {
                    return null;
                }
            });
        } catch (JSRuntimeException jsRuntimeException) {
            jsRuntimeException.printStackTrace();
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_jsexecutor_test);

        initJSExecutor();

        findViewById(R.id.evaluateJavaScript).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // executeJS must be done in the background Thread.
                // Here is just showcase.
                String case0 = "(function paladin() {return('hello paladin')})()";
                String case1 = "(function paladin() {return 1})()";
                String case2 = "output.log('dd')";
                String case3 = "var nf = new Intl.NumberFormat(undefined, {style:'currency', currency:'GBP'});\n" +
                        "    nf.format(100);";
                String case4 = "function test(a1, a2, a3, a4) {return a1 + a2 + a3 + a4;}";
                String result = null;
                try {
                    result = getJSExecutor().evaluateStringScript(case0, "");
                } catch (JSRuntimeException jsRuntimeException) {
                    jsRuntimeException.printStackTrace();
                }
                if (result != null) {
                    Toast.makeText(getApplicationContext(), result, Toast.LENGTH_LONG).show();
                }
            }
        });

        findViewById(R.id.callJavaScriptFunc).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                PLDNativeArray args = new PLDNativeArray(3);
                args.putString("Hell");
                args.putDouble(2.0f);
                args.putInt(2);
                try {
                    JSValue jsValue = getJSExecutor().callJavaScriptFunc("", "paladinLog", args);
                    if (jsValue != null && jsValue.isString()) {
                        Toast.makeText(getApplicationContext(), jsValue.toString(), Toast.LENGTH_LONG).show();
                    }
                } catch (JSRuntimeException jsRuntimeException) {
                    jsRuntimeException.printStackTrace();
                }
            }
        });

        findViewById(R.id.injectKVObject).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String str = injectKVObject();
                Toast.makeText(getApplicationContext(), str, Toast.LENGTH_LONG).show();
            }
        });

        findViewById(R.id.testPaladinLog).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                testPaladinLog();
            }
        });
        findViewById(R.id.testNativeBridge).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                testNativeBridge();
            }
        });
        findViewById(R.id.testNativeSetTimeout).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                testNativeSetTimeout();
            }
        });
        findViewById(R.id.testNativeRequire).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                testNativeRequire();
            }
        });

        findViewById(R.id.testSizeToFitList).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                testNativeSizeToFitList();
            }
        });

        findViewById(R.id.btnProtectJSObj).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

            }
        });

        findViewById(R.id.btnUnprotectJSObj).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

            }
        });

        findViewById(R.id.btnGc).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                getJSExecutor().gc();
            }
        });

        findViewById(R.id.btnCallFunc).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                EditText etObjName = findViewById(R.id.etObjName);
                EditText etFuncName = findViewById(R.id.etFuncName);
                String objName = etObjName.getText().toString();
                String funcName = etFuncName.getText().toString();

                PLDNativeArray args = new PLDNativeArray(3);
                args.putDouble(2.0f);
                args.putInt(2);
                args.putString("Hell");
                Object obj = null;
                try {
                    obj = getJSExecutor().callJavaScriptFunc("Paladin", "test", args);
                } catch (JSRuntimeException jsRuntimeException) {
                    jsRuntimeException.printStackTrace();
                }
                Toast.makeText(JSCExecutorTestActivity.this, obj != null ? obj.toString() : "obj is null", Toast.LENGTH_LONG).show();
            }
        });
    }

    private String injectKVObject() {
        HashMap<String, Object> map = new HashMap<>();
        map.put("key1", 1);
        map.put("key2", 2.0f);
        map.put("key3", 3.0);
        map.put("key4", "val3");

        try {
            getJSExecutor().createKVObject("LaBuLaDuo", new JSONObject(map).toString());
        } catch (Exception ignored) {

        }

        try {
            return getJSExecutor().evaluateStringScript(
                    "(function yy(){return 'key1: ' + LaBuLaDuo.key1 + ', key2: ' + LaBuLaDuo.key2 + ', key3: ' + LaBuLaDuo.key3 + ',  key4: ' + LaBuLaDuo.key4 })()"
            , "");
        } catch (JSRuntimeException jsRuntimeException) {
            jsRuntimeException.printStackTrace();
        }
        return null;
    }

    private void testNativeSetTimeout() {
        try {
            getJSExecutor().evaluateVoidScript("nativeSetTimeout()", "");
        } catch (JSRuntimeException jsRuntimeException) {
            jsRuntimeException.printStackTrace();
        }
    }

    private void testPaladinLog() {
        try {
            getJSExecutor().evaluateVoidScript("paladinLog('test paladinLog function')", "");
        } catch (JSRuntimeException jsRuntimeException) {
            jsRuntimeException.printStackTrace();
        }
    }

    private void testNativeRequire() {
        try {
            getJSExecutor().evaluateVoidScript("nativeRequire('@hll/paladin')", "");
        } catch (JSRuntimeException jsRuntimeException) {
            jsRuntimeException.printStackTrace();
        }
    }

    private void testNativeBridge() {
        try {
            getJSExecutor().evaluateVoidScript("var ret = nativeBridge('host_01', 'vc module', 'needLayout', {'k1':'v1', 'k2':0}, 'callPCMethod'); paladinLog(ret[0] + ret[1]);", "");
        } catch (JSRuntimeException jsRuntimeException) {
            jsRuntimeException.printStackTrace();
        }
    }

    public void testNativeSizeToFitList() {
        String result = null;
        try {
            result = getJSExecutor().evaluateStringScript("nativeSizeToFitList([{\"backgroundColor\":\"#40E0D0\",\"height\":0,\"hostId\":\"host_0\",\"needSizeToFit\":1,\"numberOfLines\":0,\"sizeKey\":\"Long long long long long sizeToFit text#0#20#none#false#false#0#150\",\"text\":\"Long long long long long sizeToFit text\",\"textColor\":\"#000000\",\"textSize\":20,\"type\":1,\"viewId\":\"viewid-5\",\"width\":150,\"x\":120,\"y\":372}])", "");
        } catch (JSRuntimeException jsRuntimeException) {
            jsRuntimeException.printStackTrace();
        }
        Log.d(TAG, result);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        jsExecutor.release();
    }
}