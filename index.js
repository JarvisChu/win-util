var addon = require('bindings')('win-util-addon.node')

module.exports = function(){
  let RegisterWindowMessage = function(...props){
    return addon.RegisterWindowMessageFunc(...props)
  }
  return {RegisterWindowMessage}
}