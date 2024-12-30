package com.paladin.jsruntime.v8.debugger

object Protocol {
    /***
     * Methods in https://chromedevtools.github.io/devtools-protocol/tot/Debugger/
     */
    object Debugger {
        private const val domain = "Debugger"
        const val BreakpointResolved = "$domain.breakpointResolved"
        const val EvaluateOnCallFrame = "$domain.evaluateOnCallFrame"
        const val Enable = "$domain.enable"
        const val Pause = "$domain.pause"
        const val Paused = "$domain.paused"
        const val RemoveBreakpoint = "$domain.removeBreakpoint"
        const val Resume = "$domain.resume"
        const val Resumed = "$domain.resumed"
        const val ScriptParsed = "$domain.scriptParsed"
        const val SetAsyncCallStackDepth = "$domain.setAsyncCallStackDepth"
        const val SetBreakpointsActive = "$domain.setBreakpointsActive"
        const val SetBreakpointByUrl = "$domain.setBreakpointByUrl"
        const val SetPauseOnExceptions = "$domain.setPauseOnExceptions"
        const val SetSkipAllPauses = "$domain.setSkipAllPauses"
        const val StepInto = "$domain.stepInto"
        const val StepOut = "$domain.stepOut"
        const val StepOver = "$domain.stepOver"
        const val SetScriptSource = "$domain.setScriptSource"
    }

    /***
     * Methods in https://chromedevtools.github.io/devtools-protocol/tot/Runtime/
     */
    object Runtime{
        private const val domain = "Runtime"
        const val Enable = "$domain.enable"
        const val GetProperties = "$domain.getProperties"
        const val RunIfWaitingForDebugger = "$domain.runIfWaitingForDebugger"
    }
}