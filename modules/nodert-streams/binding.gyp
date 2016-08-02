{
  'includes': [ 'common.gypi' ],
  "targets": [
    {
      "target_name": "NodeRT_Buffer_Utils",
      "sources": [ "NodeRT.Buffer.Utils.cpp",
                  "NodeRtUtils.cpp",
                  "OpaqueWrapper.cpp"],
	  "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ],
      'libraries': [ '-lruntimeobject.lib'],
      'msvs_settings': {
        'VCCLCompilerTool': {
            'AdditionalUsingDirectories' : [
			    '%ProgramFiles(x86)%/Microsoft Visual Studio 14.0/VC/lib/store/references',
				'%ProgramFiles(x86)%/Windows Kits/10/UnionMetadata'
			],
            'AdditionalOptions': [ '/ZW'] 
        }
      }
     }
  ]
}