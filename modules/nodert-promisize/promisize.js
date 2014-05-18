// Copyright (c) Microsoft Corporation
// All rights reserved. 
//
// Licensed under the Apache License, Version 2.0 (the ""License""); you may not use this file except in compliance with the License. You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0 
//
// THIS CODE IS PROVIDED ON AN  *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED WARRANTIES OR CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE, MERCHANTABLITY OR NON-INFRINGEMENT. 
//
// See the Apache Version 2.0 License for specific language governing permissions and limitations under the License.

var RSVP = require('rsvp');

function generatePromiseCb(resolve, reject) {
  return function() {
    var args = Array.prototype.slice.call(arguments);
    var err = args.shift();
    if (err) {
      return reject(err);
    }

    return resolve.apply(this, args);
  }
}

function isAsync(obj) {
  return (typeof obj === 'function' && obj.__winRtAsync__);
}

function promisizeFunction(originalFunction) {
  return function() {
    var self = this;  
    if (typeof arguments[arguments.length - 1] === 'function') {
      return originalFunction.apply(this, arguments);
    }

    var args = Array.prototype.slice.call(arguments);
    var promise = new RSVP.Promise(function(resolve, reject) {
      var cb = generatePromiseCb(resolve, reject);
      args.push(cb);
      return originalFunction.apply(self, args);
    });

    return promise;
  };
}

function promisize(pathOrFunction) {
  var obj;
  if (typeof pathOrFunction === 'string') {
    obj = require(pathOrFunction);
  }
  else if (typeof pathOrFunction === 'function' || typeof pathOrFunction === 'object') {
    obj = pathOrFunction;
  }
  else {
    return pathOrFunction;
  }

  for (var key in obj) {
    if (obj.hasOwnProperty(key)) {
      var prop = Object.getOwnPropertyDescriptor(obj, key);
      var originalValue = prop.value;
      if (originalValue && typeof originalValue === 'function') {
        if (isAsync(originalValue)) {
          obj[key] = promisizeFunction(originalValue);
        }
        else {
          promisize(originalValue);
          if (originalValue.prototype && Object.keys(originalValue.prototype).length) {
            // if this is a constructor function go over the prototype as well
            promisize(originalValue.prototype);
          }
        }
      }
    }
  }

  return obj;
}

exports.promisize = promisize;
