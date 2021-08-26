# win-util

Utils for Windows, current function list as below:

- RegisterWindowMessage:
- SetSystemVolume
- GetSystemVolume

# Build

```bash
node-gyp configure build
```

## Test

```bash
node test/test.js
```

## Usage

```bash
npm install win-util
```

## Example

Please refer to `test/test.js`

```javascript
var winutil = require('win-util')

var msgid = winutil().RegisterWindowMessage('TaskbarCreated')
console.info('msgid:', msgid)
```