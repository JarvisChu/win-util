var addon = require('bindings')('win-util-addon.node')

module.exports = function(){
  let RegisterWindowMessage = function(...props){
    return addon.RegisterWindowMessageFunc(...props)
  }

  let SetSystemVolume = function(...props){
    return addon.SetSystemVolumeFunc(...props)
  }

  return {RegisterWindowMessage, SetSystemVolume}
}