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
#include "nan.h"
#include <v8.h>

#include "OpaqueWrapper.h"
#include "node_buffer.h"
#include "node-async.h"
#include "NodeRtUtils.h"

#include <ppltasks.h>

#include "NativeBuffer.h"

using namespace concurrency;

namespace NodeRT { namespace Buffer { namespace Utils { 

  using v8::Local;
  using v8::Value;
  using v8::Object;
  using v8::String;
	using Nan::HandleScope;

  ::Windows::Storage::Streams::IBuffer^ CreateNativeBuffer(LPVOID lpBuffer, UINT32 nNumberOfBytes)
  {
    Microsoft::WRL::ComPtr<NativeBuffer> nativeBuffer;
    Microsoft::WRL::Details::MakeAndInitialize<NativeBuffer>(&nativeBuffer, (byte *)lpBuffer, nNumberOfBytes);
    auto iinspectable = (IInspectable *)reinterpret_cast<IInspectable *>(nativeBuffer.Get());
    ::Windows::Storage::Streams::IBuffer ^buffer = reinterpret_cast<::Windows::Storage::Streams::IBuffer ^>(iinspectable);

    return buffer;
  }

  static void ToIBuffer(Nan::NAN_METHOD_ARGS_TYPE info)
  {
    HandleScope scope;

    // the constructor if else should be auto generated
    if (info.Length() == 0)
    {
      Nan::ThrowError(Nan::Error(NodeRT::Utils::NewString(L"Invalid arguments, buffer was not passed in")));
      return;
    }
    else if (info.Length() == 1 && !info[0]->IsObject())
    {
      Nan::ThrowError(Nan::Error(NodeRT::Utils::NewString(L"Invalid arguments, first argument should be a Buffer object")));
      return;
    }

    Local<Value> buffer = info[0];
    char* data = node::Buffer::Data(buffer);
    size_t bufferLength = node::Buffer::Length(buffer);

    ::Windows::Storage::Streams::IBuffer^ ibuffer = CreateNativeBuffer(data, (UINT32)bufferLength);

    Local<Value> bufferWrapper = NodeRT::CreateOpaqueWrapper(ibuffer); 

    info.GetReturnValue().Set(bufferWrapper);
  }

  void Init(Local<Object> exports)
  {
    Nan::SetMethod(exports, "toIBuffer", ToIBuffer);
  }

} } }

NAN_MODULE_INIT(init)
{
  if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
  {
    Nan::ThrowError(Nan::Error(NodeRT::Utils::NewString(L"error in CoInitializeEx()")));
    return;
  }

  NodeRT::Buffer::Utils::Init(target);
}

NODE_MODULE(NodeRT_Buffer_Utils, init)