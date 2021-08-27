var winutil = require('../')

// test RegisterWindowMessage
var msgid = winutil().RegisterWindowMessage('TaskbarCreated')
console.info('msgid:', msgid)

// test SetSystemVolume
winutil().SetSystemVolume(18)

// test GetSystemVolume
// -1, error 
var volume = winutil().GetSystemVolume()
console.info("volume:", volume)

// test StartListenSystemVolumeChange
id = winutil().StartListenSystemVolumeChange(function(volume){
    console.info("volume changed:", volume)
})
console.info("id:", id)

id1 = winutil().StartListenSystemVolumeChange(function(volume){
    console.info("volume changed1:", volume)
})
console.info("id1:", id1)

setTimeout(function () {
    console.log('stop')
    winutil().StopListenSystemVolumeChange(id)
    winutil().StopListenSystemVolumeChange(id1)
    // or using winutil().StopAllListenSystemVolumeChange()
}, 10000)