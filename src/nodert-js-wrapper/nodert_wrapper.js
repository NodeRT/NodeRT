var fs = require('fs');
var spawn = require('child_process').spawn;
var os = require('os');
var path = require('path');
var platformConfigJson = require('./platform_config.json');
var async = require('async');

var PARALLEL_JOBS_LIMIT = 1;
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

var platformConfiguration;
function getConfigForPlatform() {
  if (platformConfiguration) {
    return platformConfiguration;
  }

  if (!isWindows()) {
    throw new Error("Unsupported OS! NodeRT only supports Windows")
  }

  var ver = getWindowsVersion();
  var config;

  for (var k in platformConfigJson) {
    if (ver.startsWith(k)) {
      config = platformConfigJson[k];
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

  platformConfiguration = {
    "windowsWinMDPath": windowsWinMDPath,
    "vsVersion": config["vsVersion"]
  };

  return platformConfiguration;
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
    '--outdir', config["outputDir"], '--vs', config["vsVersion"], '--verbose'], opts);
  
  return proc;
}

function generateSingleModule(namespace, outputDir, cb) {
  if (outputDir instanceof Function) {
    cb = outputDir;
    outputDir = null;
  }
  outputDir = outputDir || getDefaultDir();
  cb = cb || Function();
  var config = getConfigForPlatform();
  config["namespace"] = namespace;
  config["outputDir"] = outputDir;
  // for now we just run the command line
  var process = spawnNodeRT(config, true);
  process.on('exit', cb);
}

function generateModules(namespaces, outputDir, cb) {
  if (outputDir instanceof Function) {
    cb = outputDir;
    outputDir = null;
  }
  cb = cb || Function();

  async.eachLimit(namespaces, PARALLEL_JOBS_LIMIT, function (namespace, funcCb) {
    try {
      generateSingleModule(namespace, outputDir, function generateModuleForEach(exitCode) {
        var err;
        if (exitCode != 0) {
          error = new Error("Failed to generate module for: ", namespace);
        }
        funcCb(err);
      })
    }
    catch (e) {
      funcCb(e);
    }
  },
  function completionCb(err) {
    cb(err);
  });
}

function generateFromPackageJson(packageJsonPath, outputDir, cb) {
  if (packageJsonPath instanceof Function) {
    cb = packageJsonPath;
    packageJsonPath = null;
  }
  else if (outputDir instanceof Function) {
    cb = outputDir;
    
    if (path.basename(packageJsonPath.toLowerCase()) != "package.json") {
      packageJsonPath = null;
      outputDir = packageJsonPath;

    }
    
    outputDir = null;
    packageJsonPath = null;
  }

  packageJsonPath = packageJsonPath || path.join(__dirname, "package.json");
  cb = cb || Function();

  var json = require(packageJsonPath);

  var modulesToGenerate
  if (json["_nodert"]) {
    modulesToGenerate = json["_nodert"]["modules"];
  }

  if (modulesToGenerate) {
    return generateModules(modulesToGenerate, outputDir, cb);
  }
  return cb(new Error("No NodeRT modules were specified in package.json"));
}

exports.generateModules = generateModules;