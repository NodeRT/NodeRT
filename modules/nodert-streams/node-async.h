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
#include <TlHelp32.h>

#include <functional>
#include <memory>
#include <vector>

namespace NodeUtils
{
  using namespace v8;
  
  typedef std::function<void (int, Handle<Value>*)> InvokeCallbackDelegate;
  
  class Async
  {
  public:
    template<typename TInput, typename TResult> 
    struct Baton 
    {
      int error_code;
      std::string error_message;

      // Custom data
      std::shared_ptr<TInput> data;
      std::shared_ptr<TResult> result;
      std::vector<Persistent<Value>> callback_args;

      void setCallbackArgs(Handle<Value>* argv, int argc)
      {
        for (int i=0; i<argc; i++)
        {
          callback_args.push_back(Persistent<Value>::New(argv[i]));
        }
      }

    private:
      uv_work_t request;
      std::function<void (Baton*)> doWork;
      std::function<void (Baton*)> afterWork;
      Persistent<Object> callbackData;

      friend Async;
    };

  private:
    class TokenData
    {
    public:
      static uv_async_t* NewAsyncToken()
      {
        uv_async_t* asyncHandle = new uv_async_t;
        uv_async_init(uv_default_loop(), asyncHandle, AsyncCb);
        SetHandleData(asyncHandle->data);

        return asyncHandle;
      }

      static uv_async_t* NewAsyncToken(
        Handle<Function> callback, 
        Handle<Value> receiver)
      {
        uv_async_t* asyncHandle = NewAsyncToken();
        SetHandleCallbackData(asyncHandle->data, callback, receiver);

        return asyncHandle;
      }

      static uv_idle_t* NewIdleToken()
      {
        uv_idle_t* idleHandle = new uv_idle_t;
        uv_idle_init(uv_default_loop(), idleHandle);

        SetHandleData(idleHandle->data);
        return idleHandle;
      }

      static uv_idle_t* NewIdleToken(
        Handle<Function> callback, 
        Handle<Value> receiver)
      {
        uv_idle_t* idleHandle = NewIdleToken();
        SetHandleCallbackData(idleHandle->data, callback, receiver);

        return idleHandle;
      }

      virtual ~TokenData()
      {
        callbackData.Dispose();
      }

    private:
      static void SetHandleData(void*& handleData)
      {
        handleData = new TokenData();
      }

      static void SetHandleCallbackData(
        void* handleData,
        Handle<Function> callback, 
        Handle<Value> receiver)
      {
        TokenData* Token = static_cast<TokenData*>(handleData);
        Token->callbackData = Persistent<Object>::New(CreateCallbackData(callback, receiver));
      }

      TokenData() {}

      Persistent<Object> callbackData;
      std::function<void ()> func;

      friend Async;
    };

  public:
    template<typename TInput, typename TResult> 
    static void __cdecl Run(
      std::shared_ptr<TInput> input, 
      std::function<void (Baton<TInput, TResult>*)> doWork, 
      std::function<void (Baton<TInput, TResult>*)> afterWork, 
      Handle<Function> callback,
      Handle<Value> receiver = Handle<Value>())
    {
      HandleScope scope;
      Handle<Object> callbackData = CreateCallbackData(callback, receiver);
      
      Baton<TInput, TResult>* baton = new Baton<TInput, TResult>();
      baton->request.data = baton;
      baton->callbackData = Persistent<Object>::New(callbackData);
      baton->error_code = 0;
      baton->data = input;
      baton->doWork = doWork;
      baton->afterWork = afterWork;

      uv_queue_work(uv_default_loop(), &baton->request, AsyncWork<TInput, TResult>, AsyncAfter<TInput, TResult>);
    }

    static uv_async_t* __cdecl GetAsyncToken()
    {
      return TokenData::NewAsyncToken();
    }

    static uv_async_t* __cdecl GetAsyncToken(
      Handle<Function> callback, 
      Handle<Value> receiver = Handle<Value>())
    {
      return TokenData::NewAsyncToken(callback, receiver);
    }

    static uv_idle_t* __cdecl GetIdleToken()
    {
      return TokenData::NewIdleToken();
    }

    static uv_idle_t* __cdecl GetIdleToken(
      Handle<Function> callback, 
      Handle<Value> receiver = Handle<Value>())
    {
      return TokenData::NewIdleToken(callback, receiver);
    }

    static void __cdecl RunOnMain(uv_async_t* async, std::function<void ()> func)
    {
      TokenData* Token = static_cast<TokenData*>(async->data);
      Token->func = func;
      uv_async_send(async);
    }

    static void __cdecl RunOnMain(std::function<void ()> func)
    {
      static unsigned int uvMainThreadId = GetMainThreadId();

      if (uvMainThreadId == uv_thread_self()) 
      {
        func();
      }
      else
      {
        uv_async_t *async = GetAsyncToken();
        RunOnMain(async, func);
      }
    }

    static void __cdecl RunCallbackOnMain(
      uv_async_t* async,
      std::function<void (InvokeCallbackDelegate invokeCallback)> func)
    {
      TokenData* Token = static_cast<TokenData*>(async->data);

      InvokeCallbackDelegate invokeCallback = [Token](int argc, Handle<Value>* argv)
      {
        if (!Token->callbackData.IsEmpty())
        {
          node::MakeCallback(Token->callbackData, String::NewSymbol("callback"), argc, argv);
        }
      };

      std::function<void ()> wrapper = [func, invokeCallback]()
      {
        HandleScope scope;
        func(invokeCallback);
      };

      RunOnMain(async, wrapper);
    }

    // defers execution of the provided function by creating an idler
    // that means, the function will be invoked once the event loop has delivered
    // all pending events.
    static void __cdecl NextTick(std::function<void ()> func)
    {
      uv_idle_t *idler = GetIdleToken();
      NextTick(idler, func);
    }

    static void __cdecl NextTick(uv_idle_t *idler, std::function<void ()> func)
    {
      TokenData* Token = static_cast<TokenData*>(idler->data);
      Token->func = func;

      uv_idle_start(idler, onNextTick);
    }

    static void __cdecl RunCallbackOnNextTick(
      uv_idle_t* idler,
      std::function<void (InvokeCallbackDelegate invokeCallback)> func)
    {
      TokenData* Token = static_cast<TokenData*>(idler->data);

      InvokeCallbackDelegate invokeCallback = [Token](int argc, Handle<Value>* argv)
      {
        if (!Token->callbackData.IsEmpty())
        {
          node::MakeCallback(Token->callbackData, String::NewSymbol("callback"), argc, argv);
        }
      };

      std::function<void ()> wrapper = [func, invokeCallback]()
      {
        HandleScope scope;
        func(invokeCallback);
      };

      NextTick(idler, wrapper);
    }

  private:
    static Handle<Object> CreateCallbackData(Handle<Function> callback, Handle<Value> receiver)
    {
      HandleScope scope;

      Handle<Object> callbackData;
      if (!callback.IsEmpty() && !callback->Equals(Undefined()))
      {
        callbackData = Object::New();
        
        if (!receiver.IsEmpty())
        {
          callbackData->SetPrototype(receiver);
        }

        callbackData->Set(String::NewSymbol("callback"), callback);
      
        // get the current domain:
        Handle<Value> currentDomain = Undefined();

        Handle<Object> process = v8::Context::GetCurrent()->Global()->Get(String::NewSymbol("process")).As<Object>();
        if (!process->Equals(Undefined()))
        {
          currentDomain = process->Get(String::NewSymbol("domain")) ;
        }

        callbackData->Set(String::NewSymbol("domain"), currentDomain);
      }

      return scope.Close(callbackData);
    };

    template<typename TInput, typename TResult> 
    static void __cdecl AsyncWork(uv_work_t* req) 
    {
      // No HandleScope!

      Baton<TInput, TResult>* baton = static_cast<Baton<TInput, TResult>*>(req->data);

      // Do work in threadpool here.
      // Set baton->error_code/message on failures.
      // Set baton->result with a final result object
      baton->doWork(baton);
    }


    template<typename TInput, typename TResult> 
    static void __cdecl AsyncAfter(uv_work_t* req, int status) 
    {
      HandleScope scope;
      Baton<TInput, TResult>* baton = static_cast<Baton<TInput, TResult>*>(req->data);

      // typical AfterWorkFunc implementation
      //if (baton->error) 
      //{
      //  Handle<Value> err = Exception::Error(...);
      //  Handle<Value> argv[] = { err };
      //  baton->setCallbackArgs(argv, _countof(argv));
      //}
      //else
      //{
      //  Handle<Value> argv[] = { Undefined(), ... };
      //  baton->setCallbackArgs(argv, _countof(argv));
      //} 

      baton->afterWork(baton);
      
      if (!baton->callbackData.IsEmpty() || !baton->callbackData->Equals(Undefined()))
      {
        // call the callback, using domains and all
        int argc = static_cast<int>(baton->callback_args.size());
        std::unique_ptr<Handle<Value>> handlesArr(new Handle<Value>[argc]);
        for (int i=0; i < argc; i++)
        {
          handlesArr.get()[i] = baton->callback_args[i];
        }

        node::MakeCallback(baton->callbackData, String::NewSymbol("callback"), argc, handlesArr.get());
      }

      baton->callbackData.Dispose();
      delete baton;
    }
    
    // called after the async handle is closed in order to free it's memory
    static void __cdecl AyncCloseCb(uv_handle_t* handle) 
    {
      if (handle != nullptr)
      {
        uv_async_t* async = reinterpret_cast<uv_async_t*>(handle);
        delete async;
      }
    }

    // Called by run on main in case we are not running on the main thread
    static void __cdecl AsyncCb(uv_async_t *handle, int status)
    {
      auto Token = static_cast<TokenData*>(handle->data);
      Token->func();
      uv_close((uv_handle_t*)handle, AyncCloseCb);
      delete Token;
    }

    // Attributes goes to http://stackoverflow.com/a/1982200/1060807 (etan)
    static unsigned int __cdecl GetMainThreadId()
    {
      const std::shared_ptr<void> hThreadSnapshot(
        CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0), CloseHandle);

      if (hThreadSnapshot.get() == INVALID_HANDLE_VALUE)
      {
        return 0;
      }

      THREADENTRY32 tEntry;
      tEntry.dwSize = sizeof(THREADENTRY32);
      DWORD result = 0;
      DWORD currentPID = GetCurrentProcessId();

      for (BOOL success = Thread32First(hThreadSnapshot.get(), &tEntry);
        !result && success && GetLastError() != ERROR_NO_MORE_FILES;
        success = Thread32Next(hThreadSnapshot.get(), &tEntry))
      {
        if (tEntry.th32OwnerProcessID == currentPID) 
        {
          result = tEntry.th32ThreadID;
        }
      }
      return result;
    }

    static void __cdecl onNextTick(uv_idle_t *handle, int status)
    {
      std::function<void ()> *func = static_cast<std::function<void ()> *>(handle->data);
      (*func)();
      delete func;
      uv_idle_stop(handle);
      delete handle;
    }
  };
}

