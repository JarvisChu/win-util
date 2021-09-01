var addon = require('bindings')('win-util-addon.node')

module.exports = function(){
  let RegisterWindowMessage = function(...props){
    return addon.RegisterWindowMessageFunc(...props)
  }

  // set volume of system default speaker or microphone
  // SetSystemVolume()/SetSystemVolume("speaker"): set speaker volume
  // SetSystemVolume("microphone"): set microphone volume
  let SetSystemVolume = function(...props){
    return addon.SetSystemVolumeFunc(...props)
  }

  // get volume of system default speaker or microphone
  // GetSystemVolume()/GetSystemVolume("speaker"): get speaker volume
  // GetSystemVolume("microphone"): get microphone volume
  // -1 error
  let GetSystemVolume = function(...props){
    return addon.GetSystemVolumeFunc(...props)
  }

  // StartListenSystemVolumeChange(function(volume){}) or StartListenSystemVolumeChange("speaker", function(volume){})
  // StartListenSystemVolumeChange("microphone", function(volume){})
  let StartListenSystemVolumeChange = function(...props){
    return addon.StartListenSystemVolumeChangeFunc(...props)
  }

  let StopListenSystemVolumeChange = function(...props){
    return addon.StopListenSystemVolumeChangeFunc(...props)
  }

  let StopAllListenSystemVolumeChange = function(...props){
    return addon.StopAllListenSystemVolumeChangeFunc(...props)
  }

  let Encrypt = function(...props){
    return addon.EncryptFunc(...props)
  }

  let Decrypt = function(...props){
    return addon.DecryptFunc(...props)
  }

  return {
    RegisterWindowMessage, 
    SetSystemVolume, 
    GetSystemVolume, 
    StartListenSystemVolumeChange,
    StopListenSystemVolumeChange,
    StopAllListenSystemVolumeChange,
    Encrypt,
    Decrypt
  }
}