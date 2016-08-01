{
  'variables': {
    'WIN_VER' : '{WinVer}',
    'USE_ADDITIONAL_WINMD' : '{UseAdditionalWinmd}'
  },
  'includes': [ 'common.gypi' ],
  "targets": [
    {
      "target_name": "binding",
      "sources": [ "_nodert_generated.cpp",
                  "NodeRtUtils.cpp",
                  "OpaqueWrapper.cpp",
                  "CollectionsConverterUtils.cpp"],
	  "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ],
      'libraries': [ '-lruntimeobject.lib'],
      'conditions': [ [
        'WIN_VER=="v8.0"', {
          'msvs_settings': {
            'VCCLCompilerTool': {
              'AdditionalUsingDirectories' : [
                '%ProgramFiles(x86)%/Microsoft SDKs/Windows/v8.0/ExtensionSDKs/Microsoft.VCLibs/11.0/References/CommonConfiguration/neutral',
                '%ProgramFiles(x86)%/Windows Kits/8.0/References/CommonConfiguration/Neutral',
                '%ProgramFiles%/Microsoft SDKs/Windows/v8.0/ExtensionSDKs/Microsoft.VCLibs/11.0/References/CommonConfiguration/neutral',
                '%ProgramFiles%/Windows Kits/8.0/References/CommonConfiguration/Neutral'
                ]
              }
            }
          }
        ],
        ['WIN_VER=="v8.1"', {
          'msvs_settings': {
            'VCCLCompilerTool': {
              'AdditionalUsingDirectories' : [
                '%ProgramFiles(x86)%/Microsoft SDKs/Windows/v8.1/ExtensionSDKs/Microsoft.VCLibs/12.0/References/CommonConfiguration/neutral',
                '%ProgramFiles(x86)%/Windows Kits/8.1/References/CommonConfiguration/Neutral',
                '%ProgramFiles%/Microsoft SDKs/Windows/v8.1/ExtensionSDKs/Microsoft.VCLibs/12.0/References/CommonConfiguration/neutral',
                '%ProgramFiles%/Windows Kits/8.1/References/CommonConfiguration/Neutral']
              }
            }
          }],
		  ['WIN_VER=="v10"', {
          'msvs_settings': {
            'VCCLCompilerTool': {
              'AdditionalUsingDirectories' : [
                '%ProgramFiles(x86)%/Microsoft Visual Studio 14.0/VC/lib/store/references',
				'%ProgramFiles(x86)%/Windows Kits/10/UnionMetadata',
				'%ProgramFiles%/Microsoft Visual Studio 14.0/VC/lib/store/references',
				"%ProgramFiles%/Windows Kits/10/UnionMetadata"]
              }
            }
          }],
          ['USE_ADDITIONAL_WINMD=="true"', {
          'msvs_settings': {
            'VCCLCompilerTool': {
              'AdditionalUsingDirectories' : [
                '{AdditionalWinmdPath}'
                ]
               }
             }
          }]
       ],
      'msvs_settings': {
        'VCCLCompilerTool': {
            'AdditionalOptions': [ '/ZW'] 
        }
      }
     }
  ]
}