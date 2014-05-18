NodeRT sample - using the device camera
=========================================

A sample script for showing a notification on the desktop.

**Perquisites**:

In order to show notifications from a desktop application you need to have a shortcut with an **app-id**
More info on this is available here: http://msdn.microsoft.com/en-us/library/windows/desktop/hh802762

You can create a shortcut in node.js easily using the following node module: https://github.com/nadavbar/node-win-shortcut/

After you've created the shortcut, replace 'node_app_id' in the following line in the script with your shortcut's app-id:

```javascript
var appId = 'node_app_id';
```

In order to run this sample you should generate the following namespaces and place them in a node_modules directory near the script file:

* Windows.Data.Xml.Dom
* Windows.Ui.Notifications