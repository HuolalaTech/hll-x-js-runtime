package com.paladin.engine

import android.content.Context
import androidx.multidex.MultiDex
import androidx.multidex.MultiDexApplication
import com.paladin.jsruntime.v8.debugger.StethoHelper

class App : MultiDexApplication() {

    override fun attachBaseContext(base: Context) {
        super.attachBaseContext(base)
        MultiDex.install(this)
    }
    override fun onCreate() {
        super.onCreate()
        StethoHelper.initializeDebugger(this)
    }
}