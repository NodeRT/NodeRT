// Copyright (c) Microsoft Corporation
// All rights reserved. 
//
// Licensed under the Apache License, Version 2.0 (the ""License""); you may not use this file except in compliance with the License. You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0 
//
// THIS CODE IS PROVIDED ON AN  *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED WARRANTIES OR CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE, MERCHANTABLITY OR NON-INFRINGEMENT. 
//
// See the Apache Version 2.0 License for specific language governing permissions and limitations under the License.

#pragma once 

#include <v8.h>
#include <node.h>
#include <string>
#include "OpaqueWrapper.h"

#define WCHART_NOT_BUILTIN_IN_NODE 1

using namespace v8;

namespace NodeRT { namespace Utils {

  v8::Local<v8::Value> WinRtExceptionToJsError(Platform::Exception^ exception)
  {
    v8::HandleScope scope;

    if (exception == nullptr)
    {
      return scope.Close(v8::Undefined());
    }

    // we use casting here in case that wchar_t is not a built-in type
    const wchar_t* errorMessage = exception->Message->Data();

    v8::Handle<v8::Value> error = v8::Exception::Error(v8::String::New(reinterpret_cast<const uint16_t*>(errorMessage)));
    error.As<v8::Object>()->Set(v8::String::NewSymbol("HRESULT"),v8::Integer::New(exception->HResult));

    return scope.Close(error);
  }

  void ThrowWinRtExceptionInJs(Platform::Exception^ exception)
  {
    if (exception == nullptr)
    {
      return;
    }

    v8::ThrowException(WinRtExceptionToJsError(exception));
  }

  // creates an object with the following structure:
  // {
  //    "callback" : [callback fuction]
  //    "domain" : [the domain in which the async function/event was called/registered] (this is optional)
  // }
  v8::Handle<v8::Object> CreateCallbackObjectInDomain(v8::Handle<v8::Function> callback)
  {
    v8::HandleScope scope;
    
    // get the current domain:
    v8::Handle<v8::Object> callbackObject = v8::Object::New();
    
    callbackObject->Set(v8::String::NewSymbol("callback"), callback);

    v8::Handle<v8::Object> process = v8::Context::GetCurrent()->Global()->Get(v8::String::NewSymbol("process")).As<v8::Object>();

    if (process.IsEmpty() || process->Equals(v8::Undefined()))
    {
      return scope.Close(callbackObject);
    }

    v8::Handle<v8::Value> currentDomain = process->Get(v8::String::NewSymbol("domain")) ;

    if (!currentDomain.IsEmpty() && !currentDomain->Equals(v8::Undefined()))
    {
      callbackObject->Set(v8::String::NewSymbol("domain"), currentDomain);  
    }

    return scope.Close(callbackObject);
  }

  // Calls the callback in the appropriate domwin, expects an object in the following format:
  // {
  //    "callback" : [callback fuction]
  //    "domain" : [the domain in which the async function/event was called/registered] (this is optional)
  // }
  v8::Handle<v8::Value> CallCallbackInDomain(v8::Handle<v8::Object> callbackObject, int argc, v8::Handle<v8::Value> argv[]) 
  {
    return node::MakeCallback(callbackObject, v8::String::NewSymbol("callback"), argc, argv);
  }

  ::Platform::Object^ GetObjectInstance(v8::Handle<v8::Value>  value)
  {
    WrapperBase* wrapper = node::ObjectWrap::Unwrap<WrapperBase>(value.As<v8::Object>());
    return wrapper->GetObjectInstance();
  }

  v8::Handle<v8::String> NewString(const wchar_t* str)
  {
#ifdef WCHART_NOT_BUILTIN_IN_NODE
    return v8::String::New(reinterpret_cast<const uint16_t*>(str));
#else
    return v8::String::New(str);
#endif
  }

  const wchar_t* StringToWchar(v8::String::Value& str)
  {
#ifdef WCHART_NOT_BUILTIN_IN_NODE
    return reinterpret_cast<const wchar_t*>(*str);
#else
    return *str;
#endif
  }

#ifndef min
  size_t min(size_t one, size_t two)
  {
    if (one < two)
    {
      return one;
    }

    return two;
  }
#endif

#ifdef WCHART_NOT_BUILTIN_IN_NODE
  // compares 2 strings using a case insensitive comparison
  bool CaseInsenstiveEquals(const wchar_t* str1, const uint16_t* str2)
  {
    int maxCount = static_cast<int>(min(wcslen(str1), wcslen(reinterpret_cast<const wchar_t*>(str2))));
    return (_wcsnicmp(str1, reinterpret_cast<const wchar_t*>(str2), maxCount) == 0);
  }
#endif

  // compares 2 strings using a case insensitive comparison
  bool CaseInsenstiveEquals(const wchar_t* str1, const wchar_t* str2)
  {
    int maxCount = static_cast<int>(min(wcslen(str1), wcslen(str2)));
    return (_wcsnicmp(str1, str2, maxCount) == 0);
  }

  void RegisterNameSpace(const char* ns, Handle<Value> nsExports)
  {
    HandleScope scope;
    Handle<Object> global = Context::GetCurrent()->Global();
    if (!global->Has(String::NewSymbol("__winRtNamespaces__")))
    {
      global->Set(String::NewSymbol("__winRtNamespaces__"), Object::New(), PropertyAttribute::DontEnum) ;
    }

    Handle<Object> nsObject = global->Get(String::NewSymbol("__winRtNamespaces__")).As<Object>();
    nsObject->Set(String::NewSymbol(ns), nsExports);
  }

  v8::Handle<v8::Value> CreateExternalWinRTObject(const char* ns, const char* objectName, ::Platform::Object ^instance)
  {
    HandleScope scope;
    Handle<Value> opaqueWrapper = CreateOpaqueWrapper(instance);

    Handle<Object> global = Context::GetCurrent()->Global();
    if (!global->Has(String::NewSymbol("__winRtNamespaces__")))
    {
      return scope.Close(opaqueWrapper);
    }

    Handle<Object> winRtObj = global->Get(String::NewSymbol("__winRtNamespaces__")).As<Object>();

    Handle<String> nsSymbol = String::NewSymbol(ns);
    if (!winRtObj->Has(nsSymbol))
    {
      return scope.Close(opaqueWrapper);
    }

    Handle<Object> nsObject = winRtObj->Get(nsSymbol).As<Object>();

    Handle<String> objectNameSymbol = String::NewSymbol(objectName);
    if (!nsObject->Has(objectNameSymbol))
    {
      return scope.Close(opaqueWrapper);
    }

    Handle<Function> objectFunc = nsObject->Get(objectNameSymbol).As<Function>();
    Handle<Value> args[] = {opaqueWrapper};
    return scope.Close(objectFunc->NewInstance(_countof(args), args));
  }

  ::Windows::Foundation::TimeSpan TimeSpanFromMilli(int64_t millis)
  {
    ::Windows::Foundation::TimeSpan timeSpan;
    timeSpan.Duration = millis * 10000;

    return timeSpan;
  }

  ::Windows::Foundation::DateTime DateTimeFromJSDate(v8::Handle<v8::Value> value)
  {
    ::Windows::Foundation::DateTime time;
    if (value->IsDate())
    {
      // 116444736000000000 = The time in 100 nanoseconds between 1/1/1970(UTC) to 1/1/1601(UTC)
      // ux_time = (Current time since 1601 in 100 nano sec units)/10000 - 116444736000000000;
      time.UniversalTime = value->IntegerValue()* 10000 + 116444736000000000;
    }
    return time; 
  }

  bool StrToGuid(v8::Handle<v8::Value> value, LPCLSID guid)
  {
    if (value.IsEmpty() || !value->IsString())
    {
      return false;
    }

    v8::String::Value stringVal(value);
    std::wstring guidStr( L"{" );
    guidStr += StringToWchar(stringVal);
    guidStr += L"}" ;

    HRESULT hr = CLSIDFromString( guidStr.c_str(), guid );
    if( FAILED( hr ) )
    {
      return false;
    }

    return true;
  }

  bool IsGuid(v8::Handle<v8::Value> value)
  {
    GUID guid;
    return StrToGuid(value, &guid);
  }

  ::Platform::Guid GuidFromJs(v8::Handle<v8::Value> value)
  {
    GUID guid;
    if(!StrToGuid(value, &guid))
    {
      return ::Platform::Guid();
    }

    return ::Platform::Guid(guid);
  }

  Handle<Value> GuidToJs(::Platform::Guid guid)
  {
    OLECHAR* bstrGuid;
    StringFromCLSID(guid, &bstrGuid);
    
    Handle<String> strVal = NewString(bstrGuid);
    CoTaskMemFree(bstrGuid);
    return strVal;
  }

  Handle<Value> ColorToJs(::Windows::UI::Color color)
  {
    HandleScope scope;
    Handle<Object> obj = Object::New();

    obj->Set(String::NewSymbol("G"), Integer::NewFromUnsigned(color.G));
    obj->Set(String::NewSymbol("B"), Integer::NewFromUnsigned(color.B));
    obj->Set(String::NewSymbol("A"), Integer::NewFromUnsigned(color.A));
    obj->Set(String::NewSymbol("R"), Integer::NewFromUnsigned(color.R));

    return scope.Close(obj);
  }

  ::Windows::UI::Color ColorFromJs(Handle<Value> value)
  {
    ::Windows::UI::Color retVal;
    if (!value->IsObject())
    {
      return retVal;
    }

    Handle<Object> obj = value.As<Object>();
    if (!obj->Has(String::NewSymbol("G")))
    {
      retVal.G = static_cast<unsigned char>(obj->Get(String::NewSymbol("G"))->Uint32Value());
    }

    if (!obj->Has(String::NewSymbol("A")))
    {
      retVal.G = static_cast<unsigned char>(obj->Get(String::NewSymbol("A"))->Uint32Value());
    }

    if (!obj->Has(String::NewSymbol("B")))
    {
      retVal.G = static_cast<unsigned char>(obj->Get(String::NewSymbol("B"))->Uint32Value());
    }

    if (!obj->Has(String::NewSymbol("R")))
    {
      retVal.G = static_cast<unsigned char>(obj->Get(String::NewSymbol("R"))->Uint32Value());
    }

    return retVal;
  }

  bool IsColor(Handle<Value> value)
  {
    if (!value->IsObject())
    {
      return false;
    }

    Handle<Object> obj = value.As<Object>();
    if (!obj->Has(String::NewSymbol("G")))
    {
      return false;
    }

    if (!obj->Has(String::NewSymbol("A")))
    {
      return false;
    }

    if (!obj->Has(String::NewSymbol("B")))
    {
      return false;
    }

    if (!obj->Has(String::NewSymbol("R")))
    {
      return false;
    }

    return true;
  }

  v8::Handle<v8::Value> RectToJs(::Windows::Foundation::Rect rect)
  {
    HandleScope scope;
    Handle<Object> obj = Object::New();

    obj->Set(String::NewSymbol("bottom"), Number::New(rect.Bottom));
    obj->Set(String::NewSymbol("height"), Number::New(rect.Height));
    obj->Set(String::NewSymbol("left"), Number::New(rect.Left));
    obj->Set(String::NewSymbol("right"), Number::New(rect.Right));
    obj->Set(String::NewSymbol("top"), Number::New(rect.Top));
    obj->Set(String::NewSymbol("width"), Number::New(rect.Width));
    obj->Set(String::NewSymbol("x"), Number::New(rect.X));
    obj->Set(String::NewSymbol("y"), Number::New(rect.Y));

    return scope.Close(obj);
  }

  ::Windows::Foundation::Rect RectFromJs(v8::Handle<v8::Value> value)
  {
    ::Windows::Foundation::Rect rect;  

    if (!value->IsObject())
    {
      return rect;
    }

    Handle<Object> obj = value.As<Object>();

    if (obj->Has(String::NewSymbol("x")))
    {
      rect.X = static_cast<float>(obj->Get(String::NewSymbol("x"))->NumberValue());
    }

    if (obj->Has(String::NewSymbol("y")))
    {
      rect.Y = static_cast<float>(obj->Get(String::NewSymbol("y"))->NumberValue());
    }

    if (obj->Has(String::NewSymbol("height")))
    {
      rect.Height = static_cast<float>(obj->Get(String::NewSymbol("height"))->NumberValue());
    }

    if (obj->Has(String::NewSymbol("width")))
    {
      rect.Width = static_cast<float>(obj->Get(String::NewSymbol("width"))->NumberValue());
    }

    return rect;
  }

  bool IsRect(v8::Handle<v8::Value> value)
  {
    if (!value->IsObject())
    {
      return false;
    }

    Handle<Object> obj = value.As<Object>();

    if (!obj->Has(String::NewSymbol("x")))
    {
      return false;
    }

    if (!obj->Has(String::NewSymbol("y")))
    {
      return false;
    }

    if (!obj->Has(String::NewSymbol("height")))
    {
      return false;
    }

    if (!obj->Has(String::NewSymbol("width")))
    {
      return false;
    }

    return true;
  }

  v8::Handle<v8::Value> PointToJs(::Windows::Foundation::Point point)
  {
    HandleScope scope;
    Handle<Object> obj = Object::New();

    obj->Set(String::NewSymbol("x"), Number::New(point.X));
    obj->Set(String::NewSymbol("y"), Number::New(point.Y));

    return scope.Close(obj);
  }

  ::Windows::Foundation::Point PointFromJs(v8::Handle<v8::Value> value)
  {
    ::Windows::Foundation::Point point;  

    if (!value->IsObject())
    {
      return point;
    }

    Handle<Object> obj = value.As<Object>();

    if (obj->Has(String::NewSymbol("x")))
    {
      point.X = static_cast<float>(obj->Get(String::NewSymbol("x"))->NumberValue());
    }

    if (obj->Has(String::NewSymbol("y")))
    {
      point.Y = static_cast<float>(obj->Get(String::NewSymbol("y"))->NumberValue());
    }

    return point;
  }

  bool IsPoint(v8::Handle<v8::Value> value)
  {
    if (!value->IsObject())
    {
      return false;
    }

    Handle<Object> obj = value.As<Object>();

    if (!obj->Has(String::NewSymbol("x")))
    {
      return false;
    }

    if (!obj->Has(String::NewSymbol("y")))
    {
      return false;
    }

    return true;
  }

  v8::Handle<v8::Value> SizeToJs(::Windows::Foundation::Size size)
  {
    HandleScope scope;
    Handle<Object> obj = Object::New();

    obj->Set(String::NewSymbol("height"), Number::New(size.Height));
    obj->Set(String::NewSymbol("width"), Number::New(size.Width));

    return scope.Close(obj);
  }

  ::Windows::Foundation::Size SizeFromJs(v8::Handle<v8::Value> value)
  {
    ::Windows::Foundation::Size size;  

    if (!value->IsObject())
    {
      return size;
    }

    Handle<Object> obj = value.As<Object>();

    if (obj->Has(String::NewSymbol("height")))
    {
      size.Height = static_cast<float>(obj->Get(String::NewSymbol("height"))->NumberValue());
    }

    if (obj->Has(String::NewSymbol("width")))
    {
      size.Width = static_cast<float>(obj->Get(String::NewSymbol("width"))->NumberValue());
    }

    return size;
  }

  bool IsSize(v8::Handle<v8::Value> value)
  {
    if (!value->IsObject())
    {
      return false;
    }

    Handle<Object> obj = value.As<Object>();

    if (!obj->Has(String::NewSymbol("height")))
    {
      return false;
    }

    if (!obj->Has(String::NewSymbol("width")))
    {
      return false;
    }

    return true;
  }

  wchar_t GetFirstChar(v8::Handle<v8::Value> value)
  {
    wchar_t retVal = 0;

    if (!value->IsString())
    {
      return retVal;
    }

    Handle<String> str = value.As<String>();
    if (str->Length() == 0)
    {
      return retVal;
    }

    String::Value val(str);
    retVal = (*val)[0];
    return retVal;
  }

  v8::Handle<v8::Value> JsStringFromChar(wchar_t value)
  {
    wchar_t str[2];
    str[0] = value;
    str[1] = L'\0';

    return NewString(str);
  }

} } 