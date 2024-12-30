package com.paladin.jsruntime.v8.debugger

import android.content.Context
import android.os.HandlerThread
import android.util.Log
import com.facebook.stetho.InspectorModulesProvider
import com.facebook.stetho.Stetho
import com.facebook.stetho.common.LogRedirector
import com.facebook.stetho.inspector.console.RuntimeReplFactory
import com.facebook.stetho.inspector.protocol.ChromeDevtoolsDomain
import com.paladin.jsruntime.v8.debugger.utils.logger
import java.lang.ref.WeakReference
import java.util.*
import com.facebook.stetho.inspector.protocol.module.Debugger as FacebookDebuggerStub
import com.facebook.stetho.inspector.protocol.module.Runtime as FacebookRuntimeBase


object StethoHelper {
    private var debugger: Debugger? = null
    private var runtime: Runtime? = null

    private var v8MessengerRef: WeakReference<V8Messenger>? = null
    private var handlerThreadRef: WeakReference<HandlerThread>? = null

    var scriptSourceProvider: SimpleScriptProvider = SimpleScriptProvider()

    class SimpleScriptProvider : ScriptSourceProvider {

        private val map = mutableMapOf<String, String>()
        override val allScriptIds = mutableSetOf<String>()

        fun put(scriptId: String, script: String) {
            allScriptIds.add(scriptId)
            map[scriptId] = script
        }
        override fun getSource(scriptId: String): String {
            return if (map.containsKey(scriptId)) {
                map[scriptId] ?: ""
            } else {
                ""
            }
        }
    }

    //following assumes, that some JS sources are different per user
    var scriptsPathPrefix = ""
        set(value) {
            field = "/$value/"
        }

    /**
     * Initialize Stetho to enable Chrome DevTools to be intercepted
     */
    @JvmStatic
    fun initializeDebugger(context: Context) {
        val initializer = Stetho.newInitializerBuilder(context)
            .enableDumpapp(Stetho.defaultDumperPluginsProvider(context))
            .enableWebKitInspector(defaultInspectorModulesProvider(context, scriptSourceProvider))
            .build()
        Stetho.initialize(initializer)

        scriptsPathPrefix = "user" + Random().nextInt(10) + "/"

        LogRedirector.setLogger(object : LogRedirector.Logger {
            override fun isLoggable(tag: String, priority: Int): Boolean {
                return true
            }

            override fun log(priority: Int, tag: String, message: String) {
                when (priority) {
                    Log.ERROR -> Log.e(tag, message)
                    Log.WARN -> Log.w(tag, message)
                    Log.INFO -> Log.i(tag, message)
                    Log.DEBUG -> Log.d(tag, message)
                    else -> Log.v(tag, message)
                }
            }
        })

    }

    /**
     * @return Similar to [Stetho.defaultInspectorModulesProvider] but contains [Debugger] for [V8]
     */
    @JvmStatic
    fun defaultInspectorModulesProvider(
        context: Context,
        scriptSourceProvider: ScriptSourceProvider
    ): InspectorModulesProvider {
        return InspectorModulesProvider { getInspectorModules(context, scriptSourceProvider,
            v8MessengerRef?.get()?.getV8Executor()?.let {
                V8RuntimeReplFactory(it)
            })}
    }

    @JvmOverloads
    fun getInspectorModules(
        context: Context,
        scriptSourceProvider: ScriptSourceProvider,
        factory: RuntimeReplFactory? = null
    ): Iterable<ChromeDevtoolsDomain> {
        return try {
            getDefaultInspectorModulesWithDebugger(context, scriptSourceProvider, factory)
        } catch (e: Throwable) { //v8 throws Error instead of Exception on wrong thread access, etc.
            logger.e(
                Debugger.TAG,
                "Unable to init Stetho with V8 Debugger. Default set-up will be used",
                e
            )

            getDefaultInspectorModules(context, factory)
        }
    }

    private fun getDefaultInspectorModulesWithDebugger(
        context: Context,
        scriptSourceProvider: ScriptSourceProvider,
        factory: RuntimeReplFactory? = null
    ): Iterable<ChromeDevtoolsDomain> {
        val defaultInspectorModules = getDefaultInspectorModules(context, factory)

        //remove work-around when https://github.com/facebook/stetho/pull/600 is merged
        val inspectorModules = ArrayList<ChromeDevtoolsDomain>()
        for (defaultModule in defaultInspectorModules) {
            if (FacebookDebuggerStub::class != defaultModule::class
                && FacebookRuntimeBase::class != defaultModule::class
            ) {
                inspectorModules.add(defaultModule)
            }
        }

        debugger = Debugger(scriptSourceProvider)
        runtime = Runtime(factory)
        inspectorModules.add(debugger!!)
        inspectorModules.add(runtime!!)

        bindV8ToChromeDebuggerIfReady()

        return inspectorModules
    }

    fun initializeWithV8Messenger(v8Messenger: V8Messenger, handlerThread: HandlerThread) {
        v8MessengerRef = WeakReference(v8Messenger)
        handlerThreadRef = WeakReference(handlerThread)

        bindV8ToChromeDebuggerIfReady()
    }

    /**
     * Inform Chrome DevTools, that scripts are changed. Currently closes Chrome DevTools.
     * New content will be displayed when it will be opened again.
     */
    fun notifyScriptsChanged() {
        debugger?.onScriptsChanged()
    }

    private fun bindV8ToChromeDebuggerIfReady() {
        val v8Messenger = v8MessengerRef?.get()
        val handlerThread = handlerThreadRef?.get()
        val chromeDebuggerAttached = debugger != null && runtime != null

        if (v8Messenger == null || handlerThread == null || !chromeDebuggerAttached) return

        bindV8DebuggerToChromeDebugger(
            debugger!!,
            runtime!!,
            handlerThread,
            v8Messenger
        )
    }

    /**
     * Should be called when both Chrome debugger and V8 debugger is ready
     *  (When Chrome DevTools UI is open and V8 is created in debug mode with debugger object).
     */
    private fun bindV8DebuggerToChromeDebugger(
        chromeDebugger: Debugger,
        chromeRuntime: Runtime,
        handlerThread: HandlerThread,
        v8Messenger: V8Messenger
    ) {
        chromeDebugger.initialize(handlerThread, v8Messenger)
        chromeRuntime.initialize(v8Messenger)
    }

    /**
     * @return default Stetho.DefaultInspectorModulesBuilder
     *
     * @param context Android context, which is required to access android resources by Stetho.
     * @param factory copies behaviour of [Stetho.DefaultInspectorModulesBuilder.runtimeRepl] using [Runtime]
     */
    private fun getDefaultInspectorModules(
        context: Context,
        factory: RuntimeReplFactory?
    ): Iterable<ChromeDevtoolsDomain> {
        return Stetho.DefaultInspectorModulesBuilder(context)
            .runtimeRepl(factory)
            .finish()
    }
}
