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

  return {RegisterWindowMessage, SetSystemVolume, GetSystemVolume}
}