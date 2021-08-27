#ifndef VOLUME_CTL_H
#define VOLUME_CTL_H

#include <napi.h>
#include <Windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <functional>
#include <map>

class VolumeCtl : public IAudioEndpointVolumeCallback {
public:
    static VolumeCtl& GetInstance(){
        static VolumeCtl inst;
        return inst;
    };

    static HRESULT SetSystemVolume(int volume);
    static float GetSystemVolume();

    HRESULT RegisterControlChangeNotify(int id, std::function<void(int,int)> callback);
    void UnRegisterControlChangeNotify(int id);
    void UnRegisterAllControlChangeNotify();
    
    // IUnknown methods -- AddRef, Release, and QueryInterface
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, VOID **ppvInterface);

    // IAudioEndpointVolumeCallback: Callback method for endpoint-volume-change notifications.
    HRESULT STDMETHODCALLTYPE OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify);

private:
    VolumeCtl();
    ~VolumeCtl();
   
private:
    LONG m_cRef;
    IAudioEndpointVolume *m_pEndpointVolume = NULL;
    CRITICAL_SECTION m_cs;
    std::map<int, std::function<void(int,int)> > m_callbacks;
};

#endif