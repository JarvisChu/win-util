#include <napi.h>
#include <Windows.h>
#include <sstream>
#include <thread>
#include <limits.h>
#include "volumectl.h"

// Data structure representing our thread-safe function context.
struct TsfnContext {
  TsfnContext(Napi::Env env) /*: deferred(Napi::Promise::Deferred::New(env))*/ {
    volume = 0;

    static int gid = 0;
    gid = (gid++) % INT_MAX;
    id = gid;
  };

  // Native Promise returned to JavaScript
  //Napi::Promise::Deferred deferred;

  // Native thread
  std::thread nativeThread;

  // varible to hold volume
  int volume;

  // context id
  int id;

  Napi::ThreadSafeFunction tsfn;
};

// The thread entry point. This takes as its arguments the specific
// threadsafe-function context created inside the main thread.
void threadEntry(TsfnContext* context) {
  // This callback transforms the native addon data (int *data) to JavaScript
  // values. It also receives the treadsafe-function's registered callback, and
  // may choose to call it.
  auto callback = [](Napi::Env env, Napi::Function jsCallback, int* data) {
    jsCallback.Call({Napi::Number::New(env, *data)});
  };

  HANDLE hEventStop = CreateEvent(NULL,FALSE,FALSE,NULL);
  HRESULT hr = VolumeCtl::GetInstance().RegisterControlChangeNotify(context->id, [&](int volume, int errcode){
    printf("threadEntry volume: %d, errcode:%d\n", volume, errcode);

    if(errcode != 0){
      SetEvent(hEventStop);
      return;
    }

    context->volume = volume;
    napi_status status = context->tsfn.BlockingCall(&context->volume, callback); // Perform a call into JavaScript.
    if (status != napi_ok) { Napi::Error::Fatal("ThreadEntry","Napi::ThreadSafeNapi::Function.BlockingCall() failed");}
  });

  if(SUCCEEDED(hr)){
    // waitting to stop
    DWORD nWaitResult = WaitForSingleObject(hEventStop, INFINITE);
  }

  // Release the thread-safe function. This decrements the internal thread
  // count, and will perform finalization since the count will reach 0.
  context->tsfn.Release();
  printf("threadEntry exit, ctx.id=%d\n", context->id);
}

// The thread-safe function finalizer callback. This callback executes
// at destruction of thread-safe function, taking as arguments the finalizer
// data and threadsafe-function context.
void FinalizerCallback(Napi::Env env, void* finalizeData, TsfnContext* context) {
  printf("FinalizerCallback, ctx.id=%d\n", context->id);
  // Join the thread
  context->nativeThread.join();

  // Resolve the Promise previously returned to JS via the CreateTSFN method.
  //context->deferred.Resolve(Napi::Boolean::New(env, true));
  delete context;
}

/////////////////////////////////////////////////////////////////////////////////////

Napi::Value RegisterWindowMessageFunc(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() < 1) {
    Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
    return env.Null();
  }

  if (!info[0].IsString()) {
    Napi::TypeError::New(env, "Wrong arguments").ThrowAsJavaScriptException();
    return env.Null();
  }

  std::string msg = (std::string) info[0].ToString();
  UINT WM_ID = RegisterWindowMessageA(msg.c_str());
  return Napi::Number::New(env, WM_ID);
}

Napi::Value SetSystemVolumeFunc(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() < 1) {
    Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
    return env.Null();
  }

  if (!info[0].IsNumber()) {
    Napi::TypeError::New(env, "Wrong arguments").ThrowAsJavaScriptException();
    return env.Null();
  }

  int volume = info[0].As<Napi::Number>().Int32Value();
  printf("[SetSystemVolume] volume:%d\n", volume);
  if(volume < 0 || volume > 100 ){
    Napi::TypeError::New(env, "invalid volume value").ThrowAsJavaScriptException();
    return env.Null();
  }

  HRESULT hr = VolumeCtl::SetSystemVolume(volume);
  if(FAILED(hr)){
    std::ostringstream stream;
		stream << "set volume failed, hr=0x" << std::hex << hr;
    Napi::TypeError::New(env, stream.str().c_str()).ThrowAsJavaScriptException();
    return env.Null();
  }

  return Napi::Number::New(env, 0);
}

Napi::Value GetSystemVolumeFunc(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  float v = VolumeCtl::GetSystemVolume();
  int volume = (int) (v * 100);
  return Napi::Number::New(env, volume);
}

// Exported JavaScript function. Creates the thread-safe function and native
// thread. Promise is resolved in the thread-safe function's finalizer.
Napi::Value StartListenSystemVolumeChangeFunc(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() < 1) {
    Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
    return env.Null();
  }

  if (!info[0].IsFunction()) {
    Napi::TypeError::New(env, "Wrong arguments").ThrowAsJavaScriptException();
    return env.Null();
  }

  // Construct context data
  auto ctx = new TsfnContext(env);

  // Create a new ThreadSafeFunction.
  ctx->tsfn = Napi::ThreadSafeFunction::New(
      env,                           // Environment
      info[0].As<Napi::Function>(),  // JS function from caller
      "TSFN",                        // Resource name
      0,                             // Max queue size (0 = unlimited).
      1,                             // Initial thread count
      ctx,                           // Context,
      FinalizerCallback,             // Finalizer
      (void*)nullptr                 // Finalizer data
  );
  ctx->nativeThread = std::thread(threadEntry, ctx);

  // Return the deferred's Promise. This Promise is resolved in the thread-safe
  // function's finalizer callback.
  //return ctx->deferred.Promise();
  return Napi::Number::New(env, ctx->id);
}

Napi::Value StopListenSystemVolumeChangeFunc(const Napi::CallbackInfo& info){
  Napi::Env env = info.Env();

  if (info.Length() < 1) {
    Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
    return env.Null();
  }

  if (!info[0].IsNumber()) {
    Napi::TypeError::New(env, "Wrong arguments").ThrowAsJavaScriptException();
    return env.Null();
  }

  int id = info[0].As<Napi::Number>().Int32Value();
  VolumeCtl::GetInstance().UnRegisterControlChangeNotify(id);

  return env.Undefined();
}

Napi::Value StopAllListenSystemVolumeChangeFunc(const Napi::CallbackInfo& info){
  Napi::Env env = info.Env();
  VolumeCtl::GetInstance().UnRegisterAllControlChangeNotify();
  return env.Undefined();
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "RegisterWindowMessageFunc"), Napi::Function::New(env, RegisterWindowMessageFunc));
  exports.Set(Napi::String::New(env, "SetSystemVolumeFunc"), Napi::Function::New(env, SetSystemVolumeFunc));
  exports.Set(Napi::String::New(env, "GetSystemVolumeFunc"), Napi::Function::New(env, GetSystemVolumeFunc));
  exports.Set(Napi::String::New(env, "StartListenSystemVolumeChangeFunc"), Napi::Function::New(env, StartListenSystemVolumeChangeFunc));
  exports.Set(Napi::String::New(env, "StopListenSystemVolumeChangeFunc"), Napi::Function::New(env, StopListenSystemVolumeChangeFunc));
  exports.Set(Napi::String::New(env, "StopAllListenSystemVolumeChangeFunc"), Napi::Function::New(env, StopAllListenSystemVolumeChangeFunc));

  return exports;
}

NODE_API_MODULE(win_util_addon, Init)