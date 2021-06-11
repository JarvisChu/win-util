# win-utils

Utils for Windows

# Usage

```
npm install win-utils
```

# Example

Please refer to `test/test.js`

```javascript
var record = require('win-utils')

var msgid = winutils().RegisterWindowMessage('TaskbarCreated')
console.info('msgid:', msgid)
```