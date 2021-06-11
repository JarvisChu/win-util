# win-util

Utils for Windows

# Usage

```
npm install win-util
```

# Example

Please refer to `test/test.js`

```javascript
var winutil = require('win-util')

var msgid = winutil().RegisterWindowMessage('TaskbarCreated')
console.info('msgid:', msgid)
```