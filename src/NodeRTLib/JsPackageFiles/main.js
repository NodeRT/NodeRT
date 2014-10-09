// Copyright (c) Microsoft Corporation
// All rights reserved. 
//
// Licensed under the Apache License, Version 2.0 (the ""License""); you may not use this file except in compliance with the License. You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0 
//
// THIS CODE IS PROVIDED ON AN  *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED WARRANTIES OR CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE, MERCHANTABLITY OR NON-INFRINGEMENT. 
//
// See the Apache Version 2.0 License for specific language governing permissions and limitations under the License.
var path = require('path');
var fs = require('fs');

try {
  // this little trick makes node.js Tools for VS load intellisense for the module
  if (fs.existsSync(path.join(__dirname, '{ProjectName}.d.js)'))) {
    module.exports = require('./{ProjectName}.d.js');
  }
  module.exports = require('../build/release/binding.node');
}
catch(e) {
  throw e;
}

var externalReferencedNamespaces = [{ExternalReferencedNamespaces}];


if (externalReferencedNamespaces.length > 0) {
    var namespaceRegistry = global.__winRtNamespaces__;

    if (!namespaceRegistry) {
        namespaceRegistry = {};
        Object.defineProperty(global, '__winRtNamespaces__', {
            configurable: true,
            writable: false,
            enumerable: false,
            value: namespaceRegistry
        });
    }

    function requireNamespace(namespace) {
        var m = require(namespace.toLowerCase());
        delete namespaceRegistry[namespace];
        namespaceRegistry[namespace] = m;
        return m;
    }

    for (var i in externalReferencedNamespaces) {
        var ns = externalReferencedNamespaces[i];
        Object.defineProperty(namespaceRegistry, ns, {
            configurable: true,
            enumerable: true,
            get: requireNamespace.bind(null, ns)
        });
    }
}