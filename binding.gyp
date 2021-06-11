{
	"targets": [
		{
			"target_name": "win-utils-addon",
			"sources": [
				'<!@(ls -1 src/*.cc)',
			],
			"include_dirs": [
				"<!@(node -p \"require('node-addon-api').include\")",
				"<(module_root_dir)/src",
			],
			'cflags!': [ '-fno-exceptions' ],
			'cflags_cc!': [ '-fno-exceptions' ],
      		'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
      		'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS']
		}
	]
}
