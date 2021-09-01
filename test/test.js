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

// test Encrypt
cipher1 = winutil().Encrypt("my plain text")
cipher2 = winutil().Encrypt("my plain text again", "any text as entropy")
console.info("cipher1:", cipher1, cipher1.length)
console.info("cipher2:", cipher2, cipher2.length)

// test Decrypt
plain1 = winutil().Decrypt(cipher1)
plain2 = winutil().Decrypt(cipher2, "any text as entropy") // MUST using the same entropy used by Encrypt
console.info("plain1:", plain1, plain1.length)
console.info("plain2:", plain2, plain2.length)
 