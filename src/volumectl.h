#ifndef VOLUME_CTL_H
#define VOLUME_CTL_H

#include <napi.h>
#include <Windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <functional>
#include <map>

namespace VolumeCtl {
    HRESULT SetSystemVolume(EDataFlow edf, int volume);
    float GetSystemVolume(EDataFlow edf);

    class DeviceVolumeCtl: public IAudioEndpointVolumeCallback{
    public:
        DeviceVolumeCtl(EDataFlow e);
        virtual ~DeviceVolumeCtl();
        HRESULT RegisterControlChangeNotify(int id, std::function<void(int,int)> callback);
        void UnRegisterControlChangeNotify(int id);
        void UnRegisterAllControlChangeNotify();
    
        // IUnknown methods -- AddRef, Release, and QueryInterface
        ULONG STDMETHODCALLTYPE AddRef();
        ULONG STDMETHODCALLTYPE Release();
        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, VOID **ppvInterface);

        // IAudioEndpointVolumeCallback: Callback method for endpoint-volume-change notifications.
        HRESULT STDMETHODCALLTYPE OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify);
    protected:
        EDataFlow m_dataFlow = eRender;
        LONG m_cRef;
        IAudioEndpointVolume *m_pEndpointVolume = NULL;

        CRITICAL_SECTION m_cs;
        std::map<int, std::function<void(int,int)> > m_callbacks;
    };

    class SpeakerVolumeCtl: public DeviceVolumeCtl {
    public:
        static SpeakerVolumeCtl& GetInstance(){
            static SpeakerVolumeCtl inst;
            return inst;
        };
    private:
        SpeakerVolumeCtl():DeviceVolumeCtl(eRender){};
        ~SpeakerVolumeCtl(){};
    };

    class MicrophoneVolumeCtl: public DeviceVolumeCtl {
    public:
        static MicrophoneVolumeCtl& GetInstance(){
            static MicrophoneVolumeCtl inst;
            return inst;
        };
    private:
        MicrophoneVolumeCtl():DeviceVolumeCtl(eCapture){};
        ~MicrophoneVolumeCtl(){};
    };
}; // namespace VolumeCtl

#endif