{
  'includes': [ 'common.gypi' ],
  "targets": [
    {
      "target_name": "NodeRT_Buffer_Utils",
      "sources": [ "NodeRT.Buffer.Utils.cpp",
                  "NodeRtUtils.cpp",
                  "OpaqueWrapper.cpp"],
      'libraries': [ '-lruntimeobject.lib'],
      'msvs_settings': {
        'VCCLCompilerTool': {
            'AdditionalUsingDirectories' : ['C:/Program Files (x86)/Microsoft SDKs/Windows/v8.1/ExtensionSDKs/Microsoft.VCLibs/12.0/References/CommonConfiguration/neutral',
                                            'C:/Program Files (x86)/Windows Kits/8.1/References/CommonConfiguration/Neutral'],
            'AdditionalOptions': [ '/ZW'] 
            
        }
      }
     }
  ]
}