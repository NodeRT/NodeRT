var fs = require('fs');
var os = require('os');
var path = require('path');
var platformConfig = require('./platform_config.json')

function isWindows() {
  return os.platform() == 'win32';
}

function getWindowsVersion() {
  var versionString = os.release();
  var splittedVersionString = versionString.split('.')

  if (splittedVersionString.length <= 1)
    return splittedVersionString[0];

  return splittedVersionString[0] + "." + splittedVersionString[1];
}

function getConfigForPlatform() {
  if (!isWindows()) {
    throw new Error("Unsupported OS! NodeRT only supports Windows")
  }

  var ver = getWindowsVersion();
  var config;

  for (var k in platformConfig) {
    if (ver.startsWith(k)) {
      config = platformConfig[k];
      break;
    }
  }

  if (!config) {
    throw new Error("Unsupported Windows Version! NodeRT only supports Windows 10, 8.1 and 8.0")
  }

  var windowsWinMDPath;
  for (i in config["windowsWinMDPath"]) {
    try {
      var filePath = config["windowsWinMDPath"][i];
      // replace enviornment variables in the path with their values
      var resolvedPath = filePath.replace(/%([^%]+)%/g, function(_,n) {
        return process.env[n];
      })
      console.info(resolvedPath);
      fs.statSync(resolvedPath);
      // if this didn't throw then we are good
      windowsWinMDPath = filePath;
      break;
    }
    catch (e) {
    }
  }

  if (!windowsWinMDPath) {
    throw new Error("Can't find Windows.winmd file, please make sure that the Windows SDK is installed");
  }

  return {
    "windowsWinMDPath": windowsWinMDPath,
    "vsVersion" : config["vsVersion"]
  }
}

console.info(getConfigForPlatform());