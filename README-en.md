XJSRuntime is a highly available, highly stable, and high-performance JavaScript runtime (JS executor) that supports multiple JavaScript engines (QuickJS, JSC, V8). You can use it for various purposes. Here are some primary use cases:

1. **Cross-platform solution development**: If you're planning to develop a cross-platform solution similar to ReactNative, XJSRuntime is an essential tool. It supports multiple JS engines, and its high performance and availability make it suitable for this requirement.

2. **Embedded JS executor**: If you want to embed a JS executor in your Android application but don't want to significantly increase the package size, you can opt for the QuickJS engine. QuickJS is known for its small footprint and extremely low crash rate, making it ideal for embedded applications.

3. **Script engine for backend services**: In some backend services, you may need to dynamically execute JavaScript code. Using XJSRuntime, you can easily choose the suitable JS engine and achieve efficient script execution that works seamlessly across different platforms.

4. **High-performance computation**: For tasks that require heavy computation, you can leverage the high-performance capabilities of the V8 engine to achieve fast execution of computational tasks.

The code in this library is used in production and has been validated with millions of page views. The crash rate of the QuickJS engine is even lower than 0.001 percent.

The JS engines used in this library all come from the open-source community, ensuring high reliability. Below are some detailed implementation specifics and resource links:

### JSC (JavaScriptCore)
- The `jsc.so` being used is compiled from the ReactNative community project.
- Download link: [latest jsc-android version](https://registry.npmjs.org/jsc-android/-/jsc-android-250230.2.1.tgz)
- You can view the version information in Android logcat where `JavaScriptCore.Version` is printed.

### QuickJS
- Download link: [napi_qjs_v2.1.5](https://github.com/OrangeLab/Hummer-Virtual-JS-Engine/releases/download/2.1.5/napi_qjs_v2.1.5.tar.gz)
- Steps to compile it yourself:
	1. Environment setup: Install gn, ninja, and Android NDK.
		- [gn download link](https://chrome-infra-packages.appspot.com/dl/gn/gn/linux-amd64/+/latest)
		- [ninja download link](https://github.com/ninja-build/ninja/wiki/Pre-built-Ninja-packages)
		- [Android NDK download link](https://github.com/android/ndk/wiki/Unsupported-Downloads) (recommended version: 21.4.7075529)
	2. Clone the repository and initialize submodules:
	   ```shell
       git clone git@github.com:OrangeLab/Hummer-Virtual-JS-Engine.git
       cd Hummer-Virtual-JS-Engine
       git submodule init
       git submodule update
       ```
	3. Modify the `ndk_path` in the `BUILDCONFIG.gn` file to your local Android NDK installation path.
	4. Run the build script:
	   ```shell
       sh build-qjs-android.sh
       ```

### V8
- Download link: [v8-android 11.1000.4](https://cdn.npmmirror.com/packages/v8-android/11.1000.4/v8-android-11.1000.4.tgz)

With XJSRuntime and its powerful underlying JS engines, you can more conveniently achieve high-performance, cross-platform, and embedded JavaScript execution needs.