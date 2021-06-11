var addon = require('bindings')('win-utils-addon.node')

module.exports = function(){
  this.RegisterWindowMessage = function(...props){
    return addon.RegisterWindowMessageFunc(...props)
  }

  return this
}