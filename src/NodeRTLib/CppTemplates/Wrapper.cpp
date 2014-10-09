// Copyright (c) Microsoft Corporation
// All rights reserved. 
//
// Licensed under the Apache License, Version 2.0 (the ""License""); you may not use this file except in compliance with the License. You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0 
//
// THIS CODE IS PROVIDED ON AN  *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED WARRANTIES OR CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE, MERCHANTABLITY OR NON-INFRINGEMENT. 
//
// See the Apache Version 2.0 License for specific language governing permissions and limitations under the License.

#define NTDDI_VERSION 0x06010000

#include <v8.h>
#include <string>
#include <node_object_wrap.h>
#include <ppltasks.h>
#include "CollectionsConverter.h"
#include "CollectionsWrap.h"
#include "node-async.h"
#include "NodeRtUtils.h"
#include "OpaqueWrapper.h"
#include "WrapperBase.h"

#using <@(Model.Assembly.GetName().Name).WinMD>

// this undefs fixes the issues of compiling Windows.Data.Json, Windows.Storag.FileProperties, and Windows.Stroage.Search
// Some of the node header files brings windows definitions with the same names as some of the WinRT methods
#undef DocumentProperties
#undef GetObject
#undef CreateEvent
#undef FindText
#undef SendMessage

const char* REGISTRATION_TOKEN_MAP_PROPERTY_NAME = "__registrationTokenMap__";

using namespace v8;
using namespace node;
using namespace concurrency;

@foreach(var name in Model.Namespaces) @("namespace " + name + " { ")

@TX.CppTemplates.TypeWrapperForwardDecleration(Model)

@foreach(var en in Model.Enums) {
  @TX.CppTemplates.Enum(en);
}

@foreach(var vt in Model.ValueTypes) {
  @TX.CppTemplates.ValueType(vt);
}
@foreach(var vt in Model.ExternalReferencedValueTypes) {
  @TX.CppTemplates.ValueType(vt);
}
  
@foreach(var t in Model.Types.Values) {
  @TX.CppTemplates.Type(t);
}
@foreach(var name in Model.Namespaces) @("} ")


void init(Handle<Object> exports)
{
  if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
  {
    ThrowException(v8::Exception::Error(NodeRT::Utils::NewString(L"error in CoInitializeEx()")));
    return;
  }
  
  @{
    var namespacePrefix = "";
    foreach(var name in Model.Namespaces)
    {
      namespacePrefix += name + "::";
    }
  
    foreach(var en in Model.Enums) 
    {
  @:@(namespacePrefix)Init@(en.Name)Enum(exports);
    }
  
    foreach(var t in Model.Types.Values)
    {
  @:@(namespacePrefix)Init@(t.Name)(exports);
    }

  }
  NodeRT::Utils::RegisterNameSpace("@(Model.WinRTNamespace)", exports);
}
@{
  var moduleName = Model.Namespaces[0];
  var counter = 0;
  for (var i=1; i< Model.Namespaces.Count; i++)
  {
    moduleName += "_" + Model.Namespaces[i];
  }
}

NODE_MODULE(binding, init)