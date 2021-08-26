#include <napi.h>
#include <Windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <sstream>


#define EXIT_ON_ERROR(hr) { if (FAILED(hr)) { goto Exit; } }
#define SAFE_RELEASE(punk) { if ((punk) != NULL) { (punk)->Release(); (punk) = NULL; } }

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

HRESULT SetVolume(int volume) {
  
  HRESULT hr;
  IMMDeviceEnumerator *deviceEnumerator = NULL;
  IMMDevice *defaultDevice = NULL;
  IAudioEndpointVolume *endpointVolume = NULL;

  hr = CoInitialize(NULL);
  EXIT_ON_ERROR(hr);

  hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);
  EXIT_ON_ERROR(hr);

  hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
  EXIT_ON_ERROR(hr);
  
  hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&endpointVolume);
  EXIT_ON_ERROR(hr);

  float v = (float)volume / 100.0;
  hr = endpointVolume->SetMasterVolumeLevelScalar(v, NULL);
  EXIT_ON_ERROR(hr);

Exit:
  SAFE_RELEASE(deviceEnumerator);
  SAFE_RELEASE(defaultDevice);
  SAFE_RELEASE(endpointVolume);
  CoUninitialize();

  return hr;
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

  HRESULT hr = SetVolume(volume);
  if(FAILED(hr)){
    std::ostringstream stream;
		stream << "set volume failed, hr=0x" << std::hex << hr;
    Napi::TypeError::New(env, stream.str().c_str()).ThrowAsJavaScriptException();
    return env.Null();
  }

  return Napi::Number::New(env, 0);
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "RegisterWindowMessageFunc"), Napi::Function::New(env, RegisterWindowMessageFunc));
  exports.Set(Napi::String::New(env, "SetSystemVolumeFunc"), Napi::Function::New(env, SetSystemVolumeFunc));
  return exports;
}

NODE_API_MODULE(win_util_addon, Init)