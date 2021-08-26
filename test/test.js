var winutil = require('../')

var msgid = winutil().RegisterWindowMessage('TaskbarCreated')
console.info('msgid:', msgid)

winutil().SetSystemVolume(18)

// -1, error 
var volume = winutil().GetSystemVolume()
console.info("volume:", volume)
