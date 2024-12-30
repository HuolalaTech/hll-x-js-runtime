package com.paladin.jsruntime.v8.debugger

interface ScriptSourceProvider {

    val allScriptIds: Set<String>

    /**
     * @param scriptId id or name of the script
     *
     * @return source code of the script.
     */
    fun getSource(scriptId: String): String
}
