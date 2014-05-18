nodert-streams
=====
Turn a WinRT stream into a node.js stream.

In order to build the native bindings of this module, run the following command from inside the directory:

```
node-gyp rebuild
```

<h3>API:</h3>

* Exposes an **InputStream** object which can be initiated by passing a WinRT input stream, this object behaves like a readable nodejs stream.
* Exposes an **OutputStream** object which can be initiated by passing a WinRT input stream, this object behaves like a writeable nodejs stream.

Here is an example of using nodert-streams for piping contents of a file to stdout:

```javascript
var streams = require('windows.storage.streams');
var storage = require('windows.storage');
var nodert_streams = require('nodert-streams');
var StorageFile = storage.StorageFile;

// open a file from the documents library
storage.KnownFolders.documentsLibrary.getFileAsync('document.txt', function(err, file) {
  if (err) {
    return console.info(err);
  }
  // call openAsync in order to retrieve the stream
  file.openAsync(storage.FileAccessMode.read, function(err, fileStream) {
    if (err) {
      return console.info(err);
    }
    // convert the winrt stream to a node.js stream
    var nodeStream = streams.InputStream(fileStream);
    // pipe the stream to the stdout
    nodeStream.pipe(process.stdout);
  });
});
```
