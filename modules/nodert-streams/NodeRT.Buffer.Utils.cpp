// Copyright (c) Microsoft Corporation
// All rights reserved. 
//
// Licensed under the Apache License, Version 2.0 (the ""License""); you may not use this file except in compliance with the License. You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0 
//
// THIS CODE IS PROVIDED ON AN  *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED WARRANTIES OR CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE, MERCHANTABLITY OR NON-INFRINGEMENT. 
//
// See the Apache Version 2.0 License for specific language governing permissions and limitations under the License.

#define NTDDI_VERSION 0x06010000
#define _WIN32_WINNT 0x0600
#include <v8.h>

#include "OpaqueWrapper.h"
#include "node_buffer.h"
#include "node-async.h"
#include "NodeRtUtils.h"

#include <ppltasks.h>

#include "NativeBuffer.h"

using namespace v8;
using namespace concurrency;

namespace NodeRT { namespace Buffer { namespace Utils { 

  ::Windows::Storage::Streams::IBuffer^ CreateNativeBuffer(LPVOID lpBuffer, UINT32 nNumberOfBytes)
  {
    Microsoft::WRL::ComPtr<NativeBuffer> nativeBuffer;
    Microsoft::WRL::Details::MakeAndInitialize<NativeBuffer>(&nativeBuffer, (byte *)lpBuffer, nNumberOfBytes);
    auto iinspectable = (IInspectable *)reinterpret_cast<IInspectable *>(nativeBuffer.Get());
    ::Windows::Storage::Streams::IBuffer ^buffer = reinterpret_cast<::Windows::Storage::Streams::IBuffer ^>(iinspectable);

    return buffer;
  }

  static v8::Handle<v8::Value> ToIBuffer(const v8::Arguments& args)
  {
    HandleScope scope;

    // the constructor if else should be auto generated
    if (args.Length() == 0)
    {
      ThrowException(Exception::Error(NodeRT::Utils::NewString(L"Invalid arguments, buffer was not passed in")));
      return scope.Close(Undefined());
    }
    else if (args.Length() == 1 && !args[0]->IsObject())
    {
      ThrowException(Exception::Error(NodeRT::Utils::NewString(L"Invalid arguments, first argument should be a Buffer object")));
      return scope.Close(Undefined());
    }

    Local<Value> buffer = args[0];
    char* data = node::Buffer::Data(buffer);
    size_t bufferLength = node::Buffer::Length(buffer);

    ::Windows::Storage::Streams::IBuffer^ ibuffer = CreateNativeBuffer(data, (UINT32)bufferLength);

    Handle<Value> bufferWrapper = NodeRT::CreateOpaqueWrapper(ibuffer); 

    return scope.Close(bufferWrapper);
  }

  void Init(Handle<Object> exports)
  {
    exports->Set(String::NewSymbol("toIBuffer"), FunctionTemplate::New(ToIBuffer)->GetFunction());
  }

} } }

void init(Handle<Object> exports)
{
  if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
  {
    ThrowException(v8::Exception::Error(NodeRT::Utils::NewString(L"error in CoInitializeEx()")));
    return;
  }

  NodeRT::Buffer::Utils::Init(exports);
}

NODE_MODULE(NodeRT_Buffer_Utils, init)