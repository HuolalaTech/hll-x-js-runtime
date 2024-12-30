[English Document](README-en.md)
# XJSRuntime

XJSRuntime是一款高可用、高稳定性、高性能、支持多款JavaScript引擎（QuickJS、JSC、V8）的JavaScript运行时（JS执行器）。你可以用它来做什么？
> 1. 如果你想自研一款跨平台方案，类似：ReactNative，你一定需要它；
> 2. 如果你想在你的Android应用中嵌入一个JS执行器，但又不想增加包体积，你可以将JavaScript引擎切换为QuickJS
> 3. .....

本开源库中的的代码均在线上使用，经过百万级别PV验证，QuickJS引擎崩溃率甚至低于10万分之一


本库中使用的JS引擎均来自开源社区，可靠性很有保证，以下具体说明：

## JSC 

jsc使用的是RN开源社区项目编译的jsc.so,

项目中使用的jsc.so下载地址:  https://registry.npmjs.org/jsc-android/-/jsc-android-250230.2.1.tgz  (250230.2.1是当前使用的版本，不同版本换版本号即可)

另：初始化jsc成功后，在Android logcat中可以看到打印的jsc版本`JavaScriptCore.Version: 250230.2.1`

## QuickJS

项目中使用的qjs.so下载地址：https://github.com/OrangeLab/Hummer-Virtual-JS-Engine/releases/download/2.1.5/napi_qjs_v2.1.5.tar.gz

若需自行编译，请按照以下步骤进行:

1. 环境配置：gn, ninja, Android NDK
	
	* `gn`: <a> https://gn.googlesource.com/gn/ </a>
		
		- 下载：You can download the latest version of GN binary for [Linux](https://chrome-infra-packages.appspot.com/dl/gn/gn/linux-amd64/+/latest), [macOS](https://chrome-infra-packages.appspot.com/dl/gn/gn/mac-amd64/+/latest) and [Windows](https://chrome-infra-packages.appspot.com/dl/gn/gn/windows-amd64/+/latest) from Google's build infrastructure.
		
		- 配置环境变量：将gn路径加到系统环境变量中
		 
	* `ninja`: <a> https://github.com/ninja-build/ninja/wiki/Pre-built-Ninja-packages</a>
    
	* `Android NDK`: <a> https://github.com/android/ndk/wiki/Unsupported-Downloads</a>，版本建议：21.4.7075529

2. git clone git@github.com:OrangeLab/Hummer-Virtual-JS-Engine.git
3. git submodule init
4. git submodule update
5. 将BUILDCONFIG.gn文件所配置的ndk_path修改为你本机上Andoird NDK安装路径
6. sh build-qjs-android.sh

## V8

https://cdn.npmmirror.com/packages/v8-android/11.1000.4/v8-android-11.1000.4.tgz