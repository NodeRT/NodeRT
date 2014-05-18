NodeRT sample - using speech synthesis and streams
=========================================

A sample script for synthesizing text to speech using the Windows.Media.SpeechSynthesis API, writing the output audio stream to file using nodert_streams, and then playing it using the media player.

**Perquisites**:

This sample uses the nodert-streams module which could be found in the NodeRT modules directory.
In order to use it - clone it to your machine to a node_modules directory near the script, then build the native bindings of nodert-streams by running the following command from inside the nodert-streams directory:

```
node-gyp rebuild
```

In order to run this sample you should generate the following namespaces and place them in a node_modules directory near the script file:

* Windows.Media.SpeechSynthesis
* Windows.Storage.Streams (for nodert-streams)