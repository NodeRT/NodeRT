NodeRT
======
<H4><b>[New!] Now supporting Windows 10 and latest versions of node.js + Electron!</b></H4>

<H3>WinRT modules generator for node.js</H3>


NodeRT is a tool that automatically generates node.js Native add-on wrappers for <a href="http://en.wikipedia.org/wiki/Windows_Runtime">UWP/WinRT APIs</a>.

NodeRT automatically exposes Microsoft’s UWP/WinRT APIs to the node.js environment by generating node modules. This enables node.js developers to write code that consumes native Windows capabilities. The generated modules' APIs are (almost) the same as the <a href="http://msdn.microsoft.com/en-us/library/windows/apps/br211377.aspx">UWP/WinRT APIs listed in MSDN</a>.
NodeRT can be used to generate node.js modules both from command line (NodeRTCmd) and from its UI tool (NodeRTUI).

NodeRT is developed and released by a group of node.js enthusiasts at Microsoft.

Here is an example of using NodeRT <a href="http://msdn.microsoft.com/library/windows/apps/br225603">windows.devices.geolocation</a> module to retrieve the current location:

```javascript
var geolocation = require('windows.devices.geolocation');
var locator = new geolocation.Geolocator();

locator.getGeopositionAsync( function(err, res) {
  if (err) {
    console.error(err);
    return;
  }

  console.info('(', res.coordinate.longitude, ',',  res.coordinate.latitude, ')');
});
```

For more examples of what NodeRT can do, check out our <a href="/samples">samples section</a>.

----------
<H3>Documentation</H3>

<a href="#Prerequisites">NodeRT Prerequisites</a>

<a href="#GeneratingWithUI">Generating a NodeRT module using the UI</a>

<a href="#GeneratingWithCmd">Generating a NodeRT module using the cmd line interface</a>

<a href="#BuildingForElectron">Building for Electron</a>

<a href="#ConsumingNodeRT">Consuming a NodeRT module in node.js/Electron</a>

<a href="#License">License</a>

<a href="#Attributions">Attributions</a>

<a href="#Contribute">Contribute</a>

-----------
<a name="Prerequisites"></a> 
<H3>NodeRT Prerequisites</H3>
First, in order to use WinRT you must be running on a Windows environment that supports WinRT- meaning Windows 10, Windows 8.1, Windows 8, or Windows Server 2012.

In order to use NodeRT, make sure you have the following installed:<br>
* Visual Studio 2015, or <a href="https://www.visualstudio.com/en-us/products/visual-studio-express-vs.aspx">VS 2015 Express for Windows Desktop</a>, for generating Windows 10 compatible modules, or Visual Studio 2013/2012 for generating Windows 8.1/8 compatible modules repsectively.<br>
* node.js (version > 10.*) - from <a href="https://nodejs.org/en/">nodejs.org</a><br>
* node-gyp - make sure to get the latest version from npm by running:
```
npm install -g node-gyp
```

Next, download the latest NodeRT release from <a href="https://github.com/NodeRT/NodeRT/releases">here</a>, or clone this repository to your machine and build the NodeRT solution using Visual Studio.

-----------
<a name="GeneratingWithUI"></a> 
<H3>Generating a NodeRT module using the UI</H3>
First, launch the UI tool by running NodeRTUI.exe:<br>

![Alt Windows.Devices.Geolocation NodeRT module contents](/doc/images/nodert_screenshot.png)

Then, follow this short list of steps in order to create a NodeRT module:<br>
* Choose a WinMD file: <br>
    - For Windows 10 SDK: <br>
    ```
    c:\Program Files (x86)\Windows Kits\10\UnionMetadata\Windows.winmd
    ```
    - For Windows 8.1 SDK: <br>
    ```
    c:\Program Files (x86)\Windows Kits\8.1\References\CommonConfiguration\Neutral\Windows.winmd
    ```
    - For Windows 8.0 SDK: <br>
    ```
    c:\Program Files (x86)\Windows Kits\8.0\References\CommonConfiguration\Neutral\Windows.winmd
    ```
* Choose a namespace to generate from the list of namespaces.<br>
* Select whether you are generating a Windows 10 compatible module using VS 015, Windows 8.1 compatible module using VS2013 or a Windows 8.0 compatible module using VS2012.<br>
* Choose the output directory in which the module will be created, or just stick with the default ones.
* You're good to go, hit the Generate & Build button! A message box with (hopefully) a success message should appear shortly.<br>

-----------
<a name="GeneratingWithCmd"></a> 
<H3>Generating a NodeRT module using the cmd line interface</H3>
NodeRT modules generation is available via a cmd-line interface using the NodeRTCmd tool.

An example of generating the Windows.Devices.Geolocation namespace from the Windows 10 Windows.winmd:
```
NodeRTCmd.exe --winmd "c:\Program Files (x86)\Windows Kits\10\UnionMetadata\Windows.winmd" --codegendir c:\NodeRT\codegen --outdir c:\NodeRT\output --namespace Windows.Devices.Geolocation
```
Note that omitting the --namespace option will generate all of the namespaces in the Winmd file.

The following is the list of options that the tool supports:
```
 --winmd [path]              File path to winmd file from which the module
                             will be generated

 --namespaces                Lists all of the namespaces in the winmd file
                             (only needs --winmd)

 --namespace [namespace]     The namespace to generate from the winmd when
                             not specified , all namespaces will be generated

 --outdir [path]             The output dir in which the compiled NodeRT module
                             will be created in

 --vs [Vs2015|Vs2013|Vs2012]  Optional, VS version to use, default is Vs2015
 
 --nodefgen                  Optional, specifying this option will reult in
                             skipping the generation of TypeScript and
                             JavaScript definition files

 --nobuild                   Optional, specifying this option will result in
                             skipping the build process for the NodeRT module

 --help                      Print this help screen

```

-----------
<a name="BuildingForElectron"></a>
<H3>Building for Electron</H3>
In order to build the generated NodeRT module for Electron, you can follow the instructions in <a href="https://github.com/electron/electron/blob/master/docs/tutorial/using-native-node-modules.md#installing-modules-and-rebuilding-for-electron">here</a>. 

The easiest way to build the module for Electron, is probably to run node-gyp directly from the module folder with the appropriate cmd-line arguments.

For example, opening the cmd-line, cd-ing to the generated module directory and running:
```
node-gyp rebuild --target=1.3.1 --arch=x64 --dist-url=https://atom.io/download/atom-shell
```

Just make sure to use the correct Electron version for the "target" argument (here we used 1.3.1).

After rebuilding the module - you can copy it to your Electron app node_modules directory (in case you havn't done that already) and use it like every other node.js/Electron module.


-----------
<a name="ConsumingNodeRT"></a> 
<H3>Consuming a NodeRT module in node.js/Electron</H3>

Requiring a generated NodeRT module is just like requiring any other node.js module - if for example, you've just generated Windows.Devices.Geolocation, copy the generated windows.devices.geolocation directory from the output folder to a node_modules folder near you (or use a full path), and run:
```javascript
var geolocation = require('windows.devices.geolocation');
```

If you are working in the node console (AKA REPL),
then entering <i>geolocation</i> will result in printing the contents of the namespace:

![Alt Windows.Devices.Geolocation NodeRT module contents](/doc/images/object_contents.png)

Creating a new WinRT object is done with the new operator. In order to inspect the method and properties of the object, you can print its prototype:
For example, creating a new Geolocator object in REPL:
```javascript
var locator = new geolocation.Geolocator();
//print the prototype
locator.__proto__
```
And the output will be:

![Alt Geolocator prototype contents](/doc/images/golocation__proto.png)

(Note that property values are fetched on the fly, and hence have undefined values when printing the prototype)

<b>Classes and fields naming</b>

We use the same convention used for WinRT javascript applications:

* Class/Enum names have the first letter in upper-case

* Class/Enum fields, that is properties, methods, and events, both member and static have the first letter in lower-case, the rest of the name is according to MSDN. 

* Enums are just javascript objects with keys corresponding to the enum fields and values to the enum fields numeric values.

<b>Properties</b>

Using Properties of an object is just straight-forward javascript, for example:
```javascript
locator.reportInterval = 2000;
console.info(locator.reportInterval);
```
<b>Synchronous methods</b>

Again, straight-forward javascript, just make the call with the appropriate arguments. If there are several WinRT overloads for the method, make the call with the right set of arguments and the correct overload of the method will be called:
```javascript
var xml = require('windows.data.xml.dom');
var xmlDoc = new xml.XmlDocument();
toastXml.loadXml('<node>some text here</node>');
```

<b>Asynchronous methods</b>

Each async method accepts the same variables as are listed in the MSDN, with the addition of a completion callback as the last argument.<br>
This callback will be called when the function has finished, and will receive an error as the first argument, and the result as the second argument:
```javascript
locator.getGeopositionAsync( function(err, res) {
  // result is of type geoposition
  if (err) {
    console.error(err);
    return;
  }

  console.info('(',res.coordinate.longitude, res.coordinate.latitude, ')');
});
```

<b>Events</b>

Registering to events is done using the class' <i>on</i> method (which is equivalent to <i>addListener</i>), which receives the event name (case insensitive) and the event handler function.
For example:
```javascript
var handler = function handler(sender, eventArgs) {
  console.info('status is:', eventArgs.status); 
};
locator.on('statusChanged', handler);
```
Unregistering from an event is done the same way, using the class's <i>off</i> or <i>removeListener</i> methods. 
Just make sure to store the event handler in order to be able to use it.
```javascript
// using same event handler as in the example above
locator.off('statusChanged', handler);
```

<b>Separation into namespaces and cross namespace usage</b>

Each NodeRT module represents a single namespace. <br>
For instance, windows.storage will have a NodeRT module, and windows.storage.streams will have another NodeRT module.<br>
The reason for this separation is strictly due to performance considerations.<br>
(We didn't want to have a huge NodeRT module that will cause the memory of node.js to blow up while most of the namespaces probably won't be used in each script).

This architecture means that in case you are using a NodeRT module which contains a function, property, or event which returns an object from another namespace, then you will need to require that namespace **before** calling that function/property/event.

For example:
```javascript
var capture = require('windows.media.capture');
// we also require this module in order to be able to access device controller properties
var devices = require('windows.media.devices');

var capture = new capture.MediaCapture();
capture.initializeAsync(function (err, res) {
  if (err) {
    return console.info(err);
  }
  
  // get the device controller, its type (VideoDeviceController) is defined in the 
  // windows.media.devices namespace -  so, we had to require that namespace as well
  var deviceController = capture.videoDeviceController;
  
  // we can now use the VideoDeviceController regularly
  deviceController.brightness.trySetValue(-1);
});
```

<b>Using WinRT streams in node.js</b>

In order to support the use of WinRT streams in node.js, we have created the <a href="/modules/nodert-streams">nodert-streams</a> module, which bridges between WinRT streams and node.js streams.

This bridge enable the conversion of WinRT streams to node.js streams, such that WinRT streams could be used just as regular node.js streams.

-----------
<a name="License"></a> 
<H3>License</H3>
NodeRT is released under the Apache 2.0 license. 
For more information, please take a look at the <a href="./LICENSE">license</a> file.

-----------
<a name="Attributions"></a> 
<H3>Attributions</H3>
In order to build NodeRT we used these 2 great libraries:
* RazorTemplates - https://github.com/volkovku/RazorTemplates
* RX.NET - https://github.com/Reactive-Extensions/Rx.NET/

-----------
<a name="Contribute"></a> 
<H3>Contribute</H3>
You are welcome to send us any bugs you may find, suggestions, or any other comments.
Before sending anything, please go over the repository issues list, just to make sure that it isn't already there.

You are more than welcome to fork this repository and send us a pull request if you feel that what you've done should be included. 
