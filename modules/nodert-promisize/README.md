nodert-promisize
================

A promisize nodule for NodeRT objects.

Example:

```javascript
var promisize = require('nodert-promisize').promisize;
var geolocation = promisize('windows.devices.geolocation');
var locator = new geolocation.Geolocator();
locator.getPositionAsync(filePath)
  .then(function(res) {
    console.info('(',res.coordinate.longitude,   res.coordinate.latitude, ')');
  }, 
  function(err) {
    console.error(err);
  }
);
```

**Dependencies**

This module uses the rsvp module - https://github.com/tildeio/rsvp.js/