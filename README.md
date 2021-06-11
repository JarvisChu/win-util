# win-util

Utils for Windows

# Usage

```
npm install win-util
```

# Example

Please refer to `test/test.js`

```javascript
var winutils = require('win-util')

var msgid = winutils().RegisterWindowMessage('TaskbarCreated')
console.info('msgid:', msgid)
```