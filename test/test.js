var winutil = require('../')

// test RegisterWindowMessage
var msgid = winutil().RegisterWindowMessage('TaskbarCreated')
console.info('msgid:', msgid)

// test SetSystemVolume
winutil().SetSystemVolume(18) // set speaker volume. or using winutil().SetSystemVolume("speaker", 18) 
winutil().SetSystemVolume("microphone", 80) // set microphone volume

// test GetSystemVolume
// -1, error 
var volume = winutil().GetSystemVolume() // get speaker volume. or using winutil().GetSystemVolume("speaker") 
console.info("speaker volume:", volume)

volume = winutil().GetSystemVolume("microphone") // get microphone volume
console.info("speaker volume:", volume)

// test StartListenSystemVolumeChange
id = winutil().StartListenSystemVolumeChange(function(volume){ // listen speaker volume change, or using winutil().StartListenSystemVolumeChange("speaker", function(volume){})
    console.info("speaker volume changed:", volume)
})
console.info("id:", id)

id1 = winutil().StartListenSystemVolumeChange("microphone", function(volume){ // listen microphone volume change
    console.info("microphone volume changed:", volume)
})
console.info("id1:", id1)

setTimeout(function () {
    console.log('stop')
    winutil().StopListenSystemVolumeChange(id)
    winutil().StopListenSystemVolumeChange(id1)
    // or using winutil().StopAllListenSystemVolumeChange()
}, 10000)