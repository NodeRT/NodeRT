// Copyright (c) Microsoft Corporation
// All rights reserved. 
//
// Licensed under the Apache License, Version 2.0 (the ""License""); you may not use this file except in compliance with the License. You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0 
//
// THIS CODE IS PROVIDED ON AN  *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED WARRANTIES OR CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE, MERCHANTABLITY OR NON-INFRINGEMENT. 
//
// See the Apache Version 2.0 License for specific language governing permissions and limitations under the License.

#include "OpaqueWrapper.h"
#include "NodeRtUtils.h"
using namespace v8;

v8::Persistent<v8::FunctionTemplate> NodeRT::OpaqueWrapper::s_constructorTemplate;

v8::Handle<v8::Value> NodeRT::OpaqueWrapper::New(const v8::Arguments& args)
{
  args.This()->SetHiddenValue(String::NewSymbol("__winrtOpaqueWrapper__"), True());

  return args.This();
}


void  NodeRT::OpaqueWrapper::Init()
{
  HandleScope scope;
  
  s_constructorTemplate = Persistent<FunctionTemplate>::New(FunctionTemplate::New(New));
  s_constructorTemplate->SetClassName(String::NewSymbol("OpaqueWrapper"));
  s_constructorTemplate->InstanceTemplate()->SetInternalFieldCount(1);
}

namespace NodeRT {
  class OpaqueWrapperInitializer 
  {
  public:
    OpaqueWrapperInitializer() 
    {
      NodeRT::OpaqueWrapper::Init();
    }
  };

  v8::Handle<v8::Object> CreateOpaqueWrapper(::Platform::Object^ winRtInstance)
  {
    HandleScope scope;
    if (winRtInstance == nullptr)
    {
      return scope.Close(Undefined().As<Object>());
    }

    v8::Handle<Value> args[] = {v8::Undefined()};
    
    v8::Handle<v8::Object> objectInstance = OpaqueWrapper::s_constructorTemplate->GetFunction()->NewInstance(0, args);
    if (objectInstance.IsEmpty())
    {
      return scope.Close(Undefined().As<Object>());
    }
    OpaqueWrapper* wrapperInstance = new OpaqueWrapper(winRtInstance);
    wrapperInstance->Wrap(objectInstance.As<Object>());
    return scope.Close(objectInstance);
  }
}

static NodeRT::OpaqueWrapperInitializer s_opaqueWrapperInitializer;

