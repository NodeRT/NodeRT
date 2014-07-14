// Copyright (c) Microsoft Corporation
// All rights reserved. 
//
// Licensed under the Apache License, Version 2.0 (the ""License""); you may not use this file except in compliance with the License. You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0 
//
// THIS CODE IS PROVIDED ON AN  *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED WARRANTIES OR CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE, MERCHANTABLITY OR NON-INFRINGEMENT. 
//
// See the Apache Version 2.0 License for specific language governing permissions and limitations under the License.

var Readable = require('stream').Readable;
var Writable = require('stream').Writable;
var util = require('util');
var bufferUtils = require('./build/Release/NodeRT_Buffer_Utils.node');
var streams = require('windows.storage.streams');

util.inherits(InputStream, Readable);
util.inherits(OutputStream, Writable);

function InputStream(source, options) {
  if (!(this instanceof InputStream))
    return new InputStream(source, options);

  Readable.call(this, options);

  options = options || { highWaterMark: 16 * 1024 }; 

  // source is a winrt iinputstream, such as a socket or file
  this._source = source;
  this._buffer = new Buffer(options.highWaterMark);
}

InputStream.prototype._read = function(n) {
  var self = this;
  var ibuffer = bufferUtils.toIBuffer(this._buffer);

  try {
    this._source.readAsync(ibuffer, n, 1, function(err, ibufferFinal) {
      if (err) {
        console.error('error reading: ', err);
        return self.emit('error', err);
      };
      if (ibufferFinal && ibufferFinal.length > 0) {
        var target = new Buffer(ibufferFinal.length);
        self._buffer.copy(target, 0, 0, ibufferFinal.length);
        self.push(target);
      }
      else if (ibufferFinal.length < n) {
        // signal that the stream had ended
        self.push(null);
      }
    });
  }
  catch (e) {
    self.emit('error', e);
  }
};

function OutputStream(source, options) {
  if (!(this instanceof OutputStream))
    return new OutputStream(source, options);

  Writable.call(this, options);

  // source is a winrt iinputstream, such as a socket or file
  this._source = source;

  var self = this;
}

OutputStream.prototype._write = function(chunk, encoding, callback) {
  var self = this;

  try {
    this._source.writeAsync(bufferUtils.toIBuffer(chunk), function(err, bytesWritten) {
      if (err) return callback(err);

      self._source.flushAsync(function(err) {
        return callback(err, bytesWritten);
      });
    });
  }
  catch (e) {
    self.emit('error', e);
  }
};

exports.InputStream = InputStream;
exports.OutputStream = OutputStream;
