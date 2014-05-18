// Copyright (c) Microsoft Corporation
// All rights reserved. 
//
// Licensed under the Apache License, Version 2.0 (the ""License""); you may not use this file except in compliance with the License. You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0 
//
// THIS CODE IS PROVIDED ON AN  *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED WARRANTIES OR CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE, MERCHANTABLITY OR NON-INFRINGEMENT. 
//
// See the Apache Version 2.0 License for specific language governing permissions and limitations under the License.

var assert = require('assert');
var promisize = require('../promisize');
var asyncMock = promisize.promisize(__dirname + '/asyncMock');

var cbCall = asyncMock.testAsync(null, true, function(err, res) {
  assert(err === null);
  assert(res === true);
});

assert(cbCall === true);

var promise = asyncMock.testAsync(null, true);
assert(promise !== true);

promise.then(function(res) {
  assert(res === true);
});

var promise2 = asyncMock.testAsync(new Error());
assert(promise !== true);

promise2.then(function(res) {
  assert(res === undefined);
}, function(err) {
  assert(err !== undefined);  
});

console.log('test passed.');

