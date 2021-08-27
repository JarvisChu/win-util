var events = require('events')
var addon = require('bindings')('win-util-addon.node')

module.exports = function(){
  let RegisterWindowMessage = function(...props){
    return addon.RegisterWindowMessageFunc(...props)
  }

  let SetSystemVolume = function(...props){
    return addon.SetSystemVolumeFunc(...props)
  }

  // -1 error
  let GetSystemVolume = function(...props){
    return addon.GetSystemVolumeFunc(...props)
  }

  let StartListenSystemVolumeChange = function(...props){
    return addon.StartListenSystemVolumeChangeFunc(...props)
  }

  let StopListenSystemVolumeChange = function(...props){
    return addon.StopListenSystemVolumeChangeFunc(...props)
  }

  let StopAllListenSystemVolumeChange = function(...props){
    return addon.StopAllListenSystemVolumeChangeFunc(...props)
  }

  return {
    RegisterWindowMessage, 
    SetSystemVolume, 
    GetSystemVolume, 
    StartListenSystemVolumeChange,
    StopListenSystemVolumeChange,
    StopAllListenSystemVolumeChange
  }
}