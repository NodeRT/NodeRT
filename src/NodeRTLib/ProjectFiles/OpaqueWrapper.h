// Copyright (c) Microsoft Corporation
// All rights reserved. 
//
// Licensed under the Apache License, Version 2.0 (the ""License""); you may not use this file except in compliance with the License. You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0 
//
// THIS CODE IS PROVIDED ON AN  *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED WARRANTIES OR CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE, MERCHANTABLITY OR NON-INFRINGEMENT. 
//
// See the Apache Version 2.0 License for specific language governing permissions and limitations under the License.

#pragma once

#include <node.h>
#include <v8.h>
#include <string>
#include "WrapperBase.h"
#include "node_object_wrap.h"

namespace NodeRT {
  class OpaqueWrapperInitializer;

  v8::Handle<v8::Object> CreateOpaqueWrapper(::Platform::Object^ wintRtHandle);

  class OpaqueWrapper : public WrapperBase
  {
  public:

    virtual ::Platform::Object^ GetObjectInstance() const override
    {
      return _instance;
    }

    static bool IsOpaqueWrapper(v8::Handle<v8::Value> value)
    {
      if (value.IsEmpty() || !value->IsObject())
      {
        return false;
      }

	  v8::Handle<v8::Value> hiddenVal = value.As<v8::Object>()->GetHiddenValue(v8::String::NewSymbol("__winrtOpaqueWrapper__"));
	  if (hiddenVal.IsEmpty() || !hiddenVal->IsBoolean())
	  {
		  return false;
	  }

	  return hiddenVal->Equals(v8::True());
    }

  private:

    OpaqueWrapper(::Platform::Object^ instance) : _instance(instance)
    {

    }

    static v8::Handle<v8::Value> New(const v8::Arguments& args);
    static void  Init();

  private:
    ::Platform::Object^ _instance;
    static v8::Persistent<v8::FunctionTemplate> s_constructorTemplate;

    friend OpaqueWrapperInitializer;
    friend v8::Handle<v8::Object> CreateOpaqueWrapper(::Platform::Object^ wintRtInstance);
  };
}