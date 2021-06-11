#include <napi.h>
#include <Windows.h>

Napi::Value RegisterWindowMessageFunc(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() < 1) {
    Napi::TypeError::New(env, "Wrong number of arguments")
        .ThrowAsJavaScriptException();
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

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "RegisterWindowMessageFunc"), Napi::Function::New(env, RegisterWindowMessageFunc));
  return exports;
}

NODE_API_MODULE(win_util_addon, Init)