#include "volumectl.h"

#define EXIT_ON_ERROR(hr) { if (FAILED(hr)) { goto Exit; } }
#define SAFE_RELEASE(punk) { if ((punk) != NULL) { (punk)->Release(); (punk) = NULL; } }


namespace VolumeCtl{

HRESULT SetSystemVolume(EDataFlow edf, int volume) {
    HRESULT hr;
    IMMDeviceEnumerator *deviceEnumerator = NULL;
    IMMDevice *defaultDevice = NULL;
    IAudioEndpointVolume *endpointVolume = NULL;

    hr = CoInitialize(NULL);
    EXIT_ON_ERROR(hr);

    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);
    EXIT_ON_ERROR(hr);

    hr = deviceEnumerator->GetDefaultAudioEndpoint(edf, eConsole, &defaultDevice);
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

float GetSystemVolume(EDataFlow edf) {
    HRESULT hr;
    IMMDeviceEnumerator *deviceEnumerator = NULL;
    IMMDevice *defaultDevice = NULL;
    IAudioEndpointVolume *endpointVolume = NULL;
    float currentVolume = (float)-0.01; // -1 for error

    hr = CoInitialize(NULL);
    EXIT_ON_ERROR(hr);

    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);
    EXIT_ON_ERROR(hr);

    hr = deviceEnumerator->GetDefaultAudioEndpoint(edf, eConsole, &defaultDevice);
    EXIT_ON_ERROR(hr);

    hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&endpointVolume);
    EXIT_ON_ERROR(hr);

    hr = endpointVolume->GetMasterVolumeLevelScalar(&currentVolume);
    EXIT_ON_ERROR(hr);

Exit:
    SAFE_RELEASE(deviceEnumerator);
    SAFE_RELEASE(defaultDevice);
    SAFE_RELEASE(endpointVolume);
    CoUninitialize();

    return currentVolume;
}  


DeviceVolumeCtl::DeviceVolumeCtl(EDataFlow e){
    m_cRef = 1;
    m_dataFlow = e;
    InitializeCriticalSection(&m_cs);
}

DeviceVolumeCtl::~DeviceVolumeCtl(){
    SAFE_RELEASE(m_pEndpointVolume);
    DeleteCriticalSection(&m_cs);
}

ULONG STDMETHODCALLTYPE DeviceVolumeCtl::AddRef(){
     return InterlockedIncrement(&m_cRef);
}

ULONG STDMETHODCALLTYPE DeviceVolumeCtl::Release(){
    ULONG ulRef = InterlockedDecrement(&m_cRef);
    if (0 == ulRef){
            delete this;
    }
    return ulRef;
}

HRESULT STDMETHODCALLTYPE DeviceVolumeCtl::QueryInterface(REFIID riid, VOID **ppvInterface){
    if (IID_IUnknown == riid){
        AddRef();
        *ppvInterface = (IUnknown*)this;
    }else if (__uuidof(IAudioEndpointVolumeCallback) == riid){
        AddRef();
        *ppvInterface = (IAudioEndpointVolumeCallback*)this;
    }else{
        *ppvInterface = NULL;
        return E_NOINTERFACE;
    }

    return S_OK;
}

HRESULT DeviceVolumeCtl::RegisterControlChangeNotify(int id, std::function<void(int,int)> callback){
    printf("VolumeCtl::RegisterControlChangeNotify, device:%d, id=%d\n", m_dataFlow, id);
    
    EnterCriticalSection(&m_cs);
    if(m_callbacks.find(id) != m_callbacks.end()){
        printf("register failed, id repeated\n");
        LeaveCriticalSection(&m_cs);
        return E_UNEXPECTED;
    }
    m_callbacks[id] = callback;
    if(m_callbacks.size() > 1) {
        LeaveCriticalSection(&m_cs);
        return S_OK;
    }
    LeaveCriticalSection(&m_cs);

    // m_callbacks.size() == 1 means the first time, we need call RegisterControlChangeNotify

    HRESULT hr;
    IMMDeviceEnumerator *deviceEnumerator = NULL;
    IMMDevice *defaultDevice = NULL;
    //IAudioEndpointVolume *endpointVolume = NULL;

    hr = CoInitialize(NULL);
    EXIT_ON_ERROR(hr);

    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);
    EXIT_ON_ERROR(hr);

    hr = deviceEnumerator->GetDefaultAudioEndpoint(m_dataFlow, eConsole, &defaultDevice);
    EXIT_ON_ERROR(hr);

    hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&m_pEndpointVolume);
    EXIT_ON_ERROR(hr);

    hr = m_pEndpointVolume->RegisterControlChangeNotify(
                     (IAudioEndpointVolumeCallback*)this);
    EXIT_ON_ERROR(hr);

Exit:
    SAFE_RELEASE(deviceEnumerator);
    SAFE_RELEASE(defaultDevice);
    CoUninitialize();

    return hr;
}

void DeviceVolumeCtl::UnRegisterControlChangeNotify(int id){
    EnterCriticalSection(&m_cs);
    // check if existed
    auto it = m_callbacks.find(id);
    if(it == m_callbacks.end()){
        LeaveCriticalSection(&m_cs);
        return;
    }

    printf("VolumeCtl::UnRegisterControlChangeNotify, device:%d, id=%d\n", m_dataFlow, id);

    // unregister
    it->second(0,-1); // error code -1, notify to thread to stop  
    m_callbacks.erase(it);

    // UnregisterControlChangeNotify if no callbacks any more
    if(m_callbacks.size() == 0) {
        if(m_pEndpointVolume){
            m_pEndpointVolume->UnregisterControlChangeNotify((IAudioEndpointVolumeCallback*)this);
            SAFE_RELEASE(m_pEndpointVolume);
        }
    }
    LeaveCriticalSection(&m_cs);
}

void DeviceVolumeCtl::UnRegisterAllControlChangeNotify()
{
    printf("VolumeCtl::UnRegisterAllControlChangeNotify\n");

    EnterCriticalSection(&m_cs);
    for(auto it = m_callbacks.begin(); it != m_callbacks.end(); it++){
        it->second(0,-1); // error code -1, notify to thread to stop
    }

    m_callbacks.clear();

    if(m_pEndpointVolume){
        m_pEndpointVolume->UnregisterControlChangeNotify((IAudioEndpointVolumeCallback*)this);
        SAFE_RELEASE(m_pEndpointVolume);
    }
}

HRESULT STDMETHODCALLTYPE DeviceVolumeCtl::OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify){
    if (pNotify == NULL) return E_INVALIDARG;

    int volume = (int)(100*pNotify->fMasterVolume + 0.5);
    printf("[OnNotify] volume: %d\n", volume);

    EnterCriticalSection(&m_cs);
    for(auto it = m_callbacks.begin(); it != m_callbacks.end(); it++) {
        it->second(volume, 0);
    }
    LeaveCriticalSection(&m_cs);

    return S_OK;
}

} // namespace VolumeCtl