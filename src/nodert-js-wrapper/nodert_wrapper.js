var fs = require('fs');
var spawn = require('child_process').spawn;
var os = require('os');
var path = require('path');
var platformConfig = require('./platform_config.json')

var NODERT_CMD_LINE_PATH = path.join(__dirname, 'bin/NodeRTCmd.exe');

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

      fs.statSync(resolvedPath);
      // if this didn't throw then we are good
      windowsWinMDPath = resolvedPath;
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

function getDefaultDir() {
  return path.join(__dirname, 'node_modules');
}

function spawnNodeRT(config, verbose) {
  verbose = !!verbose;
  var opts;
  if (verbose) {
    opts = { stdio : 'inherit' };
  }

  proc = spawn(NODERT_CMD_LINE_PATH, ['--winmd', config["windowsWinMDPath"], '--namespace', config["namespace"],
    '--outdir', config["outputDir"], '--vs', config["vsVersion"]], opts);
  
  return proc;
}

function generateModule(namespace, outputDir) {
  outputDir = outputDir || getDefaultDir();
  var config = getConfigForPlatform();
  config["namespace"] = namespace;
  config["outputDir"] = outputDir;
  // for now we just run the command line
  return spawnNodeRT(config, true);
}


generateModule("windows.devices.geolocation");