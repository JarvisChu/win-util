var winutils = require('../')

var msgid = winutils().RegisterWindowMessage('TaskbarCreated')
console.info('msgid:', msgid)