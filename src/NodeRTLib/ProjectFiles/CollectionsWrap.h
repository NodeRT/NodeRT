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
#include "WrapperBase.h"
#include "OpaqueWrapper.h"
#include "NodeRtUtils.h"

#include <functional>

namespace NodeRT {
	namespace Collections {

		using namespace v8;

		template <class T>
		class ArrayWrapper : NodeRT::WrapperBase
		{
		public:

			static v8::Handle<v8::Value> Init()
			{
				HandleScope scope;

				s_constructorTemplate = Persistent<FunctionTemplate>::New(FunctionTemplate::New(New));
				s_constructorTemplate->SetClassName(String::NewSymbol("Windows::Foundation::Array"));
				s_constructorTemplate->InstanceTemplate()->SetInternalFieldCount(1);
				s_constructorTemplate->InstanceTemplate()->SetIndexedPropertyHandler(Get, Set);

				s_constructorTemplate->PrototypeTemplate()->SetAccessor(String::NewSymbol("length"), LengthGetter);

				return scope.Close(Undefined());
			}

			static Handle<Value> CreateArrayWrapper(::Platform::Array<T>^ winRtInstance,
				std::function<Handle<Value>(T)> getterFunc = nullptr,
				std::function<bool(Handle<Value>)> checkTypeFunc = nullptr,
				std::function<T(Handle<Value>)> convertToTypeFunc = nullptr)
			{
				HandleScope scope;
				if (winRtInstance == nullptr)
				{
					return scope.Close(Undefined());
				}

				if (s_constructorTemplate.IsEmpty())
				{
					Init();
				}

				v8::Handle<Value> args [] = { v8::Undefined() };

				v8::Handle<v8::Object> objectInstance = s_constructorTemplate->GetFunction()->NewInstance(0, args);
				if (objectInstance.IsEmpty())
				{
					return scope.Close(Undefined());
				}

				ArrayWrapper<T> *wrapperInstance = new ArrayWrapper<T>(winRtInstance, getterFunc, checkTypeFunc, convertToTypeFunc);
				wrapperInstance->Wrap(objectInstance);
				return scope.Close(objectInstance);
			}

			virtual ::Platform::Object^ GetObjectInstance() const override
			{
				return _instance;
			}

		private:

			ArrayWrapper(::Platform::Array<T>^ winRtInstance,
				std::function<Handle<Value>(T)> getterFunc,
				std::function<bool(Handle<Value>)> checkTypeFunc = nullptr,
				std::function<T(Handle<Value>)> convertToTypeFunc = nullptr) :
				_instance(winRtInstance),
				_getterFunc(getterFunc),
				_checkTypeFunc(checkTypeFunc),
				_convertToTypeFunc(convertToTypeFunc)
			{

			}

			static v8::Handle<v8::Value> New(const v8::Arguments& args)
			{
				args.This()->SetHiddenValue(String::NewSymbol("__winRtInstance__"), True());

				return args.This();
			}

			static Handle<Value> LengthGetter(Local<String> property, const AccessorInfo &info)
			{
				HandleScope scope;
				if (!NodeRT::Utils::IsWinRtWrapperOf<::Platform::Array<T>^>(info.This()))
				{
					return scope.Close(Undefined());
				}

				ArrayWrapper<T>* wrapper = ArrayWrapper<T>::Unwrap<ArrayWrapper<T>>(info.This());

				try
				{
					unsigned int result = wrapper->_instance->Length;
					return scope.Close(Integer::New(result));
				}
				catch (Platform::Exception ^exception)
				{
					NodeRT::Utils::ThrowWinRtExceptionInJs(exception);
					return scope.Close(Undefined());
				}
			}

			static Handle<Value> Get(uint32_t index, const AccessorInfo &info)
			{
				HandleScope scope;
				if (!NodeRT::Utils::IsWinRtWrapperOf<::Platform::Array<T>^>(info.This()))
				{
					return scope.Close(Undefined());
				}

				ArrayWrapper<T>* wrapper = ArrayWrapper<T>::Unwrap<ArrayWrapper<T>>(info.This());

				if (wrapper->_instance->Length <= index)
				{
					return scope.Close(Undefined());
				}

				if (wrapper->_getterFunc == nullptr)
				{
					return CreateOpaqueWrapper(wrapper->_instance[index]);
				}
				else
				{
					return wrapper->_getterFunc(wrapper->_instance[index]);
				}
			}

			static Handle<Value> Set(uint32_t index, Local<Value> value, const AccessorInfo& info)
			{
				HandleScope scope;
				if (!NodeRT::Utils::IsWinRtWrapperOf<::Platform::Array<T>^>(info.This()))
				{
					return scope.Close(Undefined());
				}

				ArrayWrapper<T>* wrapper = ArrayWrapper<T>::Unwrap<ArrayWrapper<T>>(info.This());

				if (wrapper->_checkTypeFunc && !wrapper->_checkTypeFunc(value))
				{
					ThrowException(Exception::Error(NodeRT::Utils::NewString(L"The argument to set isn't of the expected type or internal WinRt object was disposed")));
					return scope.Close(Undefined());
				}

				if (wrapper->_instance->Length <= index)
				{
					ThrowException(Exception::Error(NodeRT::Utils::NewString(L"Given index exeeded array length")));
					return scope.Close(Undefined());
				}

				if (wrapper->_convertToTypeFunc)
				{
					try
					{
						wrapper->_instance[index] = wrapper->_convertToTypeFunc(value);
					}
					catch (::Platform::Exception^ e)
					{
						NodeRT::Utils::ThrowWinRtExceptionInJs(e);
					}
				}

				return scope.Close(Undefined());
			}

		private:
			::Platform::Array<T>^ _instance;
			std::function<Handle<Value>(T)> _getterFunc;
			std::function<bool(Handle<Value>)> _checkTypeFunc;
			std::function<T(Handle<Value>)> _convertToTypeFunc;
			static Persistent<FunctionTemplate> s_constructorTemplate;
		};

		template <class T>
		Persistent<FunctionTemplate> ArrayWrapper<T>::s_constructorTemplate;

		template <class T>
		class IteratorWrapper : NodeRT::WrapperBase
		{
		public:

			static v8::Handle<v8::Value> Init()
			{
				HandleScope scope;

				s_constructorTemplate = Persistent<FunctionTemplate>::New(FunctionTemplate::New(New));
				s_constructorTemplate->SetClassName(String::NewSymbol("Windows::Foundation::Collections:IIterator"));
				s_constructorTemplate->InstanceTemplate()->SetInternalFieldCount(1);

				s_constructorTemplate->PrototypeTemplate()->Set(String::NewSymbol("getMany"), FunctionTemplate::New(GetMany)->GetFunction());
				s_constructorTemplate->PrototypeTemplate()->Set(String::NewSymbol("moveNext"), FunctionTemplate::New(MoveNext)->GetFunction());

				s_constructorTemplate->PrototypeTemplate()->SetAccessor(String::NewSymbol("current"), CurrentGetter);
				s_constructorTemplate->PrototypeTemplate()->SetAccessor(String::NewSymbol("hasCurrent"), HasCurrentGetter);

				return scope.Close(Undefined());
			}

			static Handle<Value> CreateIteratorWrapper(::Windows::Foundation::Collections::IIterator<T>^ winRtInstance,
				std::function<Handle<Value>(T)> getterFunc = nullptr)
			{
				HandleScope scope;
				if (winRtInstance == nullptr)
				{
					return scope.Close(Undefined());
				}

				if (s_constructorTemplate.IsEmpty())
				{
					Init();
				}

				v8::Handle<Value> args [] = { v8::Undefined() };

				v8::Handle<v8::Object> objectInstance = s_constructorTemplate->GetFunction()->NewInstance(0, args);
				if (objectInstance.IsEmpty())
				{
					return scope.Close(Undefined());
				}

				IteratorWrapper<T> *wrapperInstance = new IteratorWrapper<T>(winRtInstance, getterFunc);
				wrapperInstance->Wrap(objectInstance);
				return scope.Close(objectInstance);
			}

			virtual ::Platform::Object^ GetObjectInstance() const override
			{
				return _instance;
			}

		private:

			IteratorWrapper(::Windows::Foundation::Collections::IIterator<T>^ winRtInstance, std::function<Handle<Value>(T)> getterFunc) :
				_instance(winRtInstance),
				_getterFunc(getterFunc)
			{

			}

			static v8::Handle<v8::Value> New(const v8::Arguments& args)
			{
				args.This()->SetHiddenValue(String::NewSymbol("__winRtInstance__"), True());

				return args.This();
			}


			static Handle<Value> MoveNext(const v8::Arguments& args)
			{
				HandleScope scope;

				if (!NodeRT::Utils::IsWinRtWrapperOf<::Windows::Foundation::Collections::IIterator<T>^>(args.This()))
				{
					return scope.Close(Undefined());
				}

				IteratorWrapper<T>* wrapper = IteratorWrapper<T>::Unwrap<IteratorWrapper<T>>(args.This());

				if (args.Length() == 0)
				{
					try
					{
						bool result;
						result = wrapper->_instance->MoveNext();
						return scope.Close(Boolean::New(result));
					}
					catch (Platform::Exception ^exception)
					{
						NodeRT::Utils::ThrowWinRtExceptionInJs(exception);
						return scope.Close(Undefined());
					}
				}
				else
				{
					ThrowException(Exception::Error(NodeRT::Utils::NewString(L"Bad arguments: no suitable overload found")));
					return scope.Close(Undefined());
				}

				return scope.Close(Undefined());
			}

			// Not supporting this for now since we need to initialize the array ourselves and don't know which size to use
			static Handle<Value> GetMany(const v8::Arguments& args)
			{
			  HandleScope scope;
        ThrowException(Exception::Error(NodeRT::Utils::NewString(L"Not implemented")));
        return scope.Close(Undefined());
			}

			static Handle<Value> CurrentGetter(Local<String> property, const AccessorInfo &info)
			{
				HandleScope scope;
				if (!NodeRT::Utils::IsWinRtWrapperOf<::Windows::Foundation::Collections::IIterator<T>^>(info.This()))
				{
					return scope.Close(Undefined());
				}

				IteratorWrapper<T>* wrapper = IteratorWrapper<T>::Unwrap<IteratorWrapper<T>>(info.This());

				try
				{
					T current = wrapper->_instance->Current;

					if (wrapper->_getterFunc != nullptr)
					{
						return scope.Close(wrapper->_getterFunc(current));
					}
					else
					{
						return scope.Close(CreateOpaqueWrapper(current));
					}
				}
				catch (Platform::Exception ^exception)
				{
					NodeRT::Utils::ThrowWinRtExceptionInJs(exception);
					return scope.Close(Undefined());
				}
			}

			static Handle<Value> HasCurrentGetter(Local<String> property, const AccessorInfo &info)
			{
				HandleScope scope;
				if (!NodeRT::Utils::IsWinRtWrapperOf<::Windows::Foundation::Collections::IIterator<T>^>(info.This()))
				{
					return scope.Close(Undefined());
				}

				IteratorWrapper<T>* wrapper = IteratorWrapper<T>::Unwrap<IteratorWrapper<T>>(info.This());

				try
				{
					bool result = wrapper->_instance->HasCurrent;
					return scope.Close(Boolean::New(result));
				}
				catch (Platform::Exception ^exception)
				{
					NodeRT::Utils::ThrowWinRtExceptionInJs(exception);
					return scope.Close(Undefined());
				}
			}

		private:
			::Windows::Foundation::Collections::IIterator<T>^ _instance;
			std::function<Handle<Value>(T)> _getterFunc;
			static Persistent<FunctionTemplate> s_constructorTemplate;
		};

    template <class T>
		class IterableWrapper : NodeRT::WrapperBase
		{
		public:

			static v8::Handle<v8::Value> Init()
			{
				HandleScope scope;

				s_constructorTemplate = Persistent<FunctionTemplate>::New(FunctionTemplate::New(New));
				s_constructorTemplate->SetClassName(String::NewSymbol("Windows::Foundation::Collections:IIterable"));
				s_constructorTemplate->InstanceTemplate()->SetInternalFieldCount(1);

				s_constructorTemplate->PrototypeTemplate()->Set(String::NewSymbol("first"), FunctionTemplate::New(First)->GetFunction());

				return scope.Close(Undefined());
			}

			static Handle<Value> CreateIterableWrapper(::Windows::Foundation::Collections::IIterable<T>^ winRtInstance,
				std::function<Handle<Value>(T)> getterFunc = nullptr)
			{
				HandleScope scope;
				if (winRtInstance == nullptr)
				{
					return scope.Close(Undefined());
				}

				if (s_constructorTemplate.IsEmpty())
				{
					Init();
				}

				v8::Handle<Value> args [] = { v8::Undefined() };

				v8::Handle<v8::Object> objectInstance = s_constructorTemplate->GetFunction()->NewInstance(0, args);
				if (objectInstance.IsEmpty())
				{
					return scope.Close(Undefined());
				}

				IterableWrapper<T> *wrapperInstance = new IterableWrapper<T>(winRtInstance, getterFunc);
				wrapperInstance->Wrap(objectInstance);
				return scope.Close(objectInstance);
			}

			virtual ::Platform::Object^ GetObjectInstance() const override
			{
				return _instance;
			}

		private:

			IterableWrapper(::Windows::Foundation::Collections::IIterable<T>^ winRtInstance, std::function<Handle<Value>(T)> getterFunc) :
				_instance(winRtInstance),
				_getterFunc(getterFunc)
			{

			}

			static v8::Handle<v8::Value> New(const v8::Arguments& args)
			{
				args.This()->SetHiddenValue(String::NewSymbol("__winRtInstance__"), True());

				return args.This();
			}


			static Handle<Value> First(const v8::Arguments& args)
			{
				HandleScope scope;

				if (!NodeRT::Utils::IsWinRtWrapperOf<::Windows::Foundation::Collections::IIterable<T>^>(args.This()))
				{
					return scope.Close(Undefined());
				}

				IterableWrapper<T>* wrapper = IterableWrapper<T>::Unwrap<IterableWrapper<T>>(args.This());

				if (args.Length() == 0)
				{
					try
					{
            ::Windows::Foundation::Collections::IIterator<T>^ result = wrapper->_instance->First();

            return scope.Close(IteratorWrapper<T>::CreateIteratorWrapper(result, wrapper->_getterFunc));
					}
					catch (Platform::Exception ^exception)
					{
						NodeRT::Utils::ThrowWinRtExceptionInJs(exception);
						return scope.Close(Undefined());
					}
				}
				else
				{
					ThrowException(Exception::Error(NodeRT::Utils::NewString(L"Bad arguments: no suitable overload found")));
					return scope.Close(Undefined());
				}

				return scope.Close(Undefined());
			}

		private:
			::Windows::Foundation::Collections::IIterable<T>^ _instance;
			std::function<Handle<Value>(T)> _getterFunc;
			static Persistent<FunctionTemplate> s_constructorTemplate;
		};

		template <class T>
		Persistent<FunctionTemplate> IterableWrapper<T>::s_constructorTemplate;

		template <class T>
		Persistent<FunctionTemplate> IteratorWrapper<T>::s_constructorTemplate;

		template <class T>
		class VectorViewWrapper : NodeRT::WrapperBase
		{
		public:

			static v8::Handle<v8::Value> Init()
			{
				HandleScope scope;

				s_constructorTemplate = Persistent<FunctionTemplate>::New(FunctionTemplate::New(New));
				s_constructorTemplate->SetClassName(String::NewSymbol("Windows::Foundation::Collections:IVectorView"));
				s_constructorTemplate->InstanceTemplate()->SetInternalFieldCount(1);
				s_constructorTemplate->InstanceTemplate()->SetIndexedPropertyHandler(Get);

				s_constructorTemplate->PrototypeTemplate()->Set(String::NewSymbol("getMany"), FunctionTemplate::New(GetMany)->GetFunction());
				s_constructorTemplate->PrototypeTemplate()->Set(String::NewSymbol("getAt"), FunctionTemplate::New(GetAt)->GetFunction());
				s_constructorTemplate->PrototypeTemplate()->Set(String::NewSymbol("indexOf"), FunctionTemplate::New(IndexOf)->GetFunction());
				s_constructorTemplate->PrototypeTemplate()->Set(String::NewSymbol("first"), FunctionTemplate::New(First)->GetFunction());

				s_constructorTemplate->PrototypeTemplate()->SetAccessor(String::NewSymbol("size"), SizeGetter);
				s_constructorTemplate->PrototypeTemplate()->SetAccessor(String::NewSymbol("length"), SizeGetter);

				return scope.Close(Undefined());
			}

			static Handle<Value> CreateVectorViewWrapper(::Windows::Foundation::Collections::IVectorView<T>^ winRtInstance,
				std::function<Handle<Value>(T)> getterFunc,
				std::function<bool(Handle<Value>)> checkTypeFunc = nullptr,
				std::function<T(Handle<Value>)> convertToTypeFunc = nullptr)
			{
				HandleScope scope;
				if (winRtInstance == nullptr)
				{
					return scope.Close(Undefined());
				}

				if (s_constructorTemplate.IsEmpty())
				{
					Init();
				}

				v8::Handle<Value> args [] = { v8::Undefined() };

				v8::Handle<v8::Object> objectInstance = s_constructorTemplate->GetFunction()->NewInstance(0, args);
				if (objectInstance.IsEmpty())
				{
					return scope.Close(Undefined());
				}

				VectorViewWrapper<T> *wrapperInstance = new VectorViewWrapper<T>(winRtInstance, getterFunc);
				wrapperInstance->Wrap(objectInstance);
				return scope.Close(objectInstance);
			}

			virtual ::Platform::Object^ GetObjectInstance() const override
			{
				return _instance;
			}

		private:

			VectorViewWrapper(::Windows::Foundation::Collections::IVectorView<T>^ winRtInstance,
				std::function<Handle<Value>(T)> getterFunc,
				std::function<bool(Handle<Value>)> checkTypeFunc = nullptr,
				std::function<T(Handle<Value>)> convertToTypeFunc = nullptr) :
				_instance(winRtInstance),
				_getterFunc(getterFunc),
				_checkTypeFunc(checkTypeFunc),
				_convertToTypeFunc(convertToTypeFunc)
			{

			}

			static v8::Handle<v8::Value> New(const v8::Arguments& args)
			{
				args.This()->SetHiddenValue(String::NewSymbol("__winRtInstance__"), True());

				return args.This();
			}

			static Handle<Value> Get(uint32_t index, const AccessorInfo &info)
			{
				HandleScope scope;
				if (!NodeRT::Utils::IsWinRtWrapperOf<::Windows::Foundation::Collections::IVectorView<T>^>(info.This()))
				{
					return scope.Close(Undefined());
				}

				VectorViewWrapper<T>* wrapper = VectorViewWrapper<T>::Unwrap<VectorViewWrapper<T>>(info.This());

				if (wrapper->_instance->Size <= index)
				{
					return scope.Close(Undefined());
				}

				if (wrapper->_getterFunc == nullptr)
				{
					return CreateOpaqueWrapper(wrapper->_instance->GetAt(index));
				}
				else
				{
					return wrapper->_getterFunc(wrapper->_instance->GetAt(index));
				}
			}


			static Handle<Value> GetAt(const v8::Arguments& args)
			{
				HandleScope scope;

				if (!NodeRT::Utils::IsWinRtWrapperOf<::Windows::Foundation::Collections::IVectorView<T>^>(args.This()))
				{
					return scope.Close(Undefined());
				}

				VectorViewWrapper<T>* wrapper = VectorViewWrapper<T>::Unwrap<VectorViewWrapper<T>>(args.This());

				if (args.Length() == 1 && args[0]->IsUint32())
				{
					try
					{
						unsigned int index = args[0]->Uint32Value();

						if (index >= wrapper->_instance->Size)
						{
							return scope.Close(Undefined());
						}
						T result;
						result = wrapper->_instance->GetAt(index);

						if (wrapper->_getterFunc)
						{
							return scope.Close(wrapper->_getterFunc(result));
						}
					}
					catch (Platform::Exception ^exception)
					{
						NodeRT::Utils::ThrowWinRtExceptionInJs(exception);
						return scope.Close(Undefined());
					}
				}
				else
				{
					ThrowException(Exception::Error(NodeRT::Utils::NewString(L"Bad arguments: no suitable overload found")));
					return scope.Close(Undefined());
				}

				return scope.Close(Undefined());
			}

      static Handle<Value> GetMany(const v8::Arguments& args)
			{
			  HandleScope scope;
        ThrowException(Exception::Error(NodeRT::Utils::NewString(L"Not implemented")));
        return scope.Close(Undefined());
			}

			static Handle<Value> First(const v8::Arguments& args)
			{
				HandleScope scope;

				if (!NodeRT::Utils::IsWinRtWrapperOf<::Windows::Foundation::Collections::IVectorView<T>^>(args.This()))
				{
					return scope.Close(Undefined());
				}

				VectorViewWrapper<T>* wrapper = VectorViewWrapper<T>::Unwrap<VectorViewWrapper<T>>(args.This());

				if (args.Length() == 0)
				{
					try
					{
						return IteratorWrapper<T>::CreateIteratorWrapper(wrapper->_instance->First(), wrapper->_getterFunc);
					}
					catch (Platform::Exception ^exception)
					{
						NodeRT::Utils::ThrowWinRtExceptionInJs(exception);
						return scope.Close(Undefined());
					}
				}
				else
				{
					ThrowException(Exception::Error(NodeRT::Utils::NewString(L"Bad arguments: no suitable overload found")));
					return scope.Close(Undefined());
				}

				return scope.Close(Undefined());
			}


			static Handle<Value> IndexOf(const v8::Arguments& args)
			{
				HandleScope scope;

				if (!NodeRT::Utils::IsWinRtWrapperOf<::Windows::Foundation::Collections::IVectorView<T>^>(args.This()))
				{
					return scope.Close(Undefined());
				}

				VectorViewWrapper<T>* wrapper = VectorViewWrapper<T>::Unwrap<VectorViewWrapper<T>>(args.This());

				if (wrapper->_convertToTypeFunc == nullptr || wrapper->_checkTypeFunc == nullptr)
				{
					ThrowException(Exception::Error(NodeRT::Utils::NewString(L"Method isn't supported")));
					return scope.Close(Undefined());
				}

				if (args.Length() == 1 && wrapper->_checkTypeFunc(args[0]))
				{
					try
					{
						T item = wrapper->_convertToTypeFunc(args[0]);

						unsigned int index;
						bool result = wrapper->_instance->IndexOf(item, &index);

						Handle<Object> resObj = Object::New();
						resObj->Set(String::NewSymbol("boolean"), Boolean::New(result));
						resObj->Set(String::NewSymbol("index"), Integer::NewFromUnsigned(index));
						return scope.Close(resObj);
					}
					catch (Platform::Exception ^exception)
					{
						NodeRT::Utils::ThrowWinRtExceptionInJs(exception);
						return scope.Close(Undefined());
					}
				}
				else
				{
					ThrowException(Exception::Error(NodeRT::Utils::NewString(L"Bad arguments: no suitable overload found")));
					return scope.Close(Undefined());
				}

				return scope.Close(Undefined());
			}

			static Handle<Value> SizeGetter(Local<String> property, const AccessorInfo &info)
			{
				HandleScope scope;
				if (!NodeRT::Utils::IsWinRtWrapperOf<::Windows::Foundation::Collections::IVectorView<T>^>(info.This()))
				{
					return scope.Close(Undefined());
				}

				VectorViewWrapper<T>* wrapper = VectorViewWrapper<T>::Unwrap<VectorViewWrapper<T>>(info.This());

				try
				{
					return Integer::NewFromUnsigned(wrapper->_instance->Size);
				}
				catch (Platform::Exception ^exception)
				{
					NodeRT::Utils::ThrowWinRtExceptionInJs(exception);
					return scope.Close(Undefined());
				}
			}

		private:
			::Windows::Foundation::Collections::IVectorView<T>^ _instance;
			std::function<Handle<Value>(T)> _getterFunc;
			std::function<bool(Handle<Value>)> _checkTypeFunc;
			std::function<T(Handle<Value>)> _convertToTypeFunc;
			static Persistent<FunctionTemplate> s_constructorTemplate;
		};

		template <class T>
		Persistent<FunctionTemplate> VectorViewWrapper<T>::s_constructorTemplate;

		template <class T>
		class VectorWrapper : NodeRT::WrapperBase
		{
		public:

			static v8::Handle<v8::Value> Init()
			{
				HandleScope scope;

				s_constructorTemplate = Persistent<FunctionTemplate>::New(FunctionTemplate::New(New));
				s_constructorTemplate->SetClassName(String::NewSymbol("Windows::Foundation::Collections:IVector"));
				s_constructorTemplate->InstanceTemplate()->SetInternalFieldCount(1);
				s_constructorTemplate->InstanceTemplate()->SetIndexedPropertyHandler(Get, Set);

				s_constructorTemplate->PrototypeTemplate()->Set(String::NewSymbol("getMany"), FunctionTemplate::New(GetMany)->GetFunction());
				s_constructorTemplate->PrototypeTemplate()->Set(String::NewSymbol("getAt"), FunctionTemplate::New(GetAt)->GetFunction());
				s_constructorTemplate->PrototypeTemplate()->Set(String::NewSymbol("indexOf"), FunctionTemplate::New(IndexOf)->GetFunction());
				s_constructorTemplate->PrototypeTemplate()->Set(String::NewSymbol("first"), FunctionTemplate::New(First)->GetFunction());
				s_constructorTemplate->PrototypeTemplate()->Set(String::NewSymbol("append"), FunctionTemplate::New(Append)->GetFunction());
				s_constructorTemplate->PrototypeTemplate()->Set(String::NewSymbol("clear"), FunctionTemplate::New(Clear)->GetFunction());
				s_constructorTemplate->PrototypeTemplate()->Set(String::NewSymbol("getView"), FunctionTemplate::New(GetView)->GetFunction());
				s_constructorTemplate->PrototypeTemplate()->Set(String::NewSymbol("insertAt"), FunctionTemplate::New(InsertAt)->GetFunction());
				s_constructorTemplate->PrototypeTemplate()->Set(String::NewSymbol("removeAt"), FunctionTemplate::New(RemoveAt)->GetFunction());
				s_constructorTemplate->PrototypeTemplate()->Set(String::NewSymbol("removeAtEnd"), FunctionTemplate::New(RemoveAtEnd)->GetFunction());
				s_constructorTemplate->PrototypeTemplate()->Set(String::NewSymbol("replaceAll"), FunctionTemplate::New(ReplaceAll)->GetFunction());
				s_constructorTemplate->PrototypeTemplate()->Set(String::NewSymbol("setAt"), FunctionTemplate::New(SetAt)->GetFunction());

				s_constructorTemplate->PrototypeTemplate()->SetAccessor(String::NewSymbol("size"), SizeGetter);
				s_constructorTemplate->PrototypeTemplate()->SetAccessor(String::NewSymbol("length"), SizeGetter);

				return scope.Close(Undefined());
			}

			static Handle<Value> CreateVectorWrapper(::Windows::Foundation::Collections::IVector<T>^ winRtInstance,
				std::function<Handle<Value>(T)> getterFunc,
				std::function<bool(Handle<Value>)> checkTypeFunc = nullptr,
				std::function<T(Handle<Value>)> convertToTypeFunc = nullptr)
			{
				HandleScope scope;
				if (winRtInstance == nullptr)
				{
					return scope.Close(Undefined());
				}

				if (s_constructorTemplate.IsEmpty())
				{
					Init();
				}

				v8::Handle<Value> args [] = { v8::Undefined() };

				v8::Handle<v8::Object> objectInstance = s_constructorTemplate->GetFunction()->NewInstance(0, args);
				if (objectInstance.IsEmpty())
				{
					return scope.Close(Undefined());
				}

				VectorWrapper<T> *wrapperInstance = new VectorWrapper<T>(winRtInstance, getterFunc, checkTypeFunc, convertToTypeFunc);
				wrapperInstance->Wrap(objectInstance);
				return scope.Close(objectInstance);
			}

			virtual ::Platform::Object^ GetObjectInstance() const override
			{
				return _instance;
			}

		private:

			VectorWrapper(::Windows::Foundation::Collections::IVector<T>^ winRtInstance,
				std::function<Handle<Value>(T)> getterFunc,
				std::function<bool(Handle<Value>)> checkTypeFunc = nullptr,
				std::function<T(Handle<Value>)> convertToTypeFunc = nullptr) :
				_instance(winRtInstance),
				_getterFunc(getterFunc),
				_checkTypeFunc(checkTypeFunc),
				_convertToTypeFunc(convertToTypeFunc)
			{

			}

			static v8::Handle<v8::Value> New(const v8::Arguments& args)
			{
				args.This()->SetHiddenValue(String::NewSymbol("__winRtInstance__"), True());

				return args.This();
			}

			static Handle<Value> Get(uint32_t index, const AccessorInfo &info)
			{
				HandleScope scope;
				if (!NodeRT::Utils::IsWinRtWrapperOf<::Windows::Foundation::Collections::IVector<T>^>(info.This()))
				{
					return scope.Close(Undefined());
				}

				VectorWrapper<T>* wrapper = VectorWrapper<T>::Unwrap<VectorWrapper<T>>(info.This());

				if (wrapper->_instance->Size <= index)
				{
					return scope.Close(Undefined());
				}

				if (wrapper->_getterFunc == nullptr)
				{
					return CreateOpaqueWrapper(wrapper->_instance->GetAt(index));
				}
				else
				{
					return wrapper->_getterFunc(wrapper->_instance->GetAt(index));
				}
			}

			static Handle<Value> Set(uint32 index, Local<Value> value, const AccessorInfo &info)
			{
				HandleScope scope;

				if (!NodeRT::Utils::IsWinRtWrapperOf<::Windows::Foundation::Collections::IVector<T>^>(info.This()))
				{
					return scope.Close(Undefined());
				}

				VectorWrapper<T>* wrapper = VectorWrapper<T>::Unwrap<VectorWrapper<T>>(info.This());

				if (!wrapper->_checkTypeFunc(value))
				{
					ThrowException(Exception::Error(NodeRT::Utils::NewString(L"The value to set isn't of the expected type")));
					return scope.Close(Undefined());
				}

				try
				{
					T item = wrapper->_convertToTypeFunc(value);

					wrapper->_instance->SetAt(index, item);
				}
				catch (Platform::Exception ^exception)
				{
					NodeRT::Utils::ThrowWinRtExceptionInJs(exception);
					return scope.Close(Undefined());
				}

				return scope.Close(Undefined());
			}


			static Handle<Value> Append(const v8::Arguments& args)
			{
				HandleScope scope;

				if (!NodeRT::Utils::IsWinRtWrapperOf<::Windows::Foundation::Collections::IVector<T>^>(args.This()))
				{
					return scope.Close(Undefined());
				}

				VectorWrapper<T>* wrapper = VectorWrapper<T>::Unwrap<VectorWrapper<T>>(args.This());

				if (args.Length() == 1 && wrapper->_checkTypeFunc(args[0]))
				{
					try
					{
						T value = wrapper->_convertToTypeFunc(args[0]);

						wrapper->_instance->Append(value);
					}
					catch (Platform::Exception ^exception)
					{
						NodeRT::Utils::ThrowWinRtExceptionInJs(exception);
						return scope.Close(Undefined());
					}
				}
				else
				{
					ThrowException(Exception::Error(NodeRT::Utils::NewString(L"Bad arguments: no suitable overload found")));
					return scope.Close(Undefined());
				}

				return scope.Close(Undefined());
			}


			static Handle<Value> Clear(const v8::Arguments& args)
			{
				HandleScope scope;

				if (!NodeRT::Utils::IsWinRtWrapperOf<::Windows::Foundation::Collections::IVector<T>^>(args.This()))
				{
					return scope.Close(Undefined());
				}

				VectorWrapper<T>* wrapper = VectorWrapper<T>::Unwrap<VectorWrapper<T>>(args.This());

				if (args.Length() == 0)
				{
					try
					{
						wrapper->_instance->Clear();
					}
					catch (Platform::Exception ^exception)
					{
						NodeRT::Utils::ThrowWinRtExceptionInJs(exception);
						return scope.Close(Undefined());
					}
				}
				else
				{
					ThrowException(Exception::Error(NodeRT::Utils::NewString(L"Bad arguments: no suitable overload found")));
					return scope.Close(Undefined());
				}

				return scope.Close(Undefined());
			}

      static Handle<Value> GetMany(const v8::Arguments& args)
			{
			  HandleScope scope;
        ThrowException(Exception::Error(NodeRT::Utils::NewString(L"Not implemented")));
        return scope.Close(Undefined());
			}

			static Handle<Value> GetView(const v8::Arguments& args)
			{
				HandleScope scope;

				if (!NodeRT::Utils::IsWinRtWrapperOf<::Windows::Foundation::Collections::IVector<T>^>(args.This()))
				{
					return scope.Close(Undefined());
				}

				VectorWrapper<T>* wrapper = VectorWrapper<T>::Unwrap<VectorWrapper<T>>(args.This());

				if (args.Length() == 0)
				{
					try
					{
						::Windows::Foundation::Collections::IVectorView<T>^ result = wrapper->_instance->GetView();
						return scope.Close(VectorViewWrapper<T>::CreateVectorViewWrapper(result,
							wrapper->_getterFunc,
							wrapper->_checkTypeFunc,
							wrapper->_convertToTypeFunc));

					}
					catch (Platform::Exception ^exception)
					{
						NodeRT::Utils::ThrowWinRtExceptionInJs(exception);
						return scope.Close(Undefined());
					}
				}
				else
				{
					ThrowException(Exception::Error(NodeRT::Utils::NewString(L"Bad arguments: no suitable overload found")));
					return scope.Close(Undefined());
				}

				return scope.Close(Undefined());
			}

			static Handle<Value> InsertAt(const v8::Arguments& args)
			{
				HandleScope scope;

				if (!NodeRT::Utils::IsWinRtWrapperOf<::Windows::Foundation::Collections::IVector<T>^>(args.This()))
				{
					return scope.Close(Undefined());
				}

				VectorWrapper<T>* wrapper = VectorWrapper<T>::Unwrap<VectorWrapper<T>>(args.This());

				if (args.Length() == 2 && args[0]->IsUint32() && wrapper->_checkTypeFunc(args[1]))
				{
					try
					{
						unsigned int index = args[0]->Uint32Value();

						T value = wrapper->_convertToTypeFunc(args[1]);
						wrapper->_instance->InsertAt(index, value);
						return scope.Close(Undefined());
					}
					catch (Platform::Exception ^exception)
					{
						NodeRT::Utils::ThrowWinRtExceptionInJs(exception);
						return scope.Close(Undefined());
					}
				}
				else
				{
					ThrowException(Exception::Error(NodeRT::Utils::NewString(L"Bad arguments: no suitable overload found")));
					return scope.Close(Undefined());
				}
			}

			static Handle<Value> RemoveAt(const v8::Arguments& args)
			{
				HandleScope scope;

				if (!NodeRT::Utils::IsWinRtWrapperOf<::Windows::Foundation::Collections::IVector<T>^>(args.This()))
				{
					return scope.Close(Undefined());
				}

				VectorWrapper<T>* wrapper = VectorWrapper<T>::Unwrap<VectorWrapper<T>>(args.This());

				if (args.Length() == 1 && args[0]->IsUint32())
				{
					try
					{
						unsigned int index = args[0]->Uint32Value();

						wrapper->_instance->RemoveAt(index);
						return scope.Close(Undefined());
					}
					catch (Platform::Exception ^exception)
					{
						NodeRT::Utils::ThrowWinRtExceptionInJs(exception);
						return scope.Close(Undefined());
					}
				}
				else
				{
					ThrowException(Exception::Error(NodeRT::Utils::NewString(L"Bad arguments: no suitable overload found")));
					return scope.Close(Undefined());
				}
			}

			static Handle<Value> RemoveAtEnd(const v8::Arguments& args)
			{
				HandleScope scope;

				if (!NodeRT::Utils::IsWinRtWrapperOf<::Windows::Foundation::Collections::IVector<T>^>(args.This()))
				{
					return scope.Close(Undefined());
				}

				VectorWrapper<T>* wrapper = VectorWrapper<T>::Unwrap<VectorWrapper<T>>(args.This());

				if (args.Length() == 0)
				{
					try
					{
						wrapper->_instance->RemoveAtEnd();
						return scope.Close(Undefined());
					}
					catch (Platform::Exception ^exception)
					{
						NodeRT::Utils::ThrowWinRtExceptionInJs(exception);
						return scope.Close(Undefined());
					}
				}
				else
				{
					ThrowException(Exception::Error(NodeRT::Utils::NewString(L"Bad arguments: no suitable overload found")));
					return scope.Close(Undefined());
				}
			}

			static Handle<Value> ReplaceAll(const v8::Arguments& args)
			{
				HandleScope scope;

				if (!NodeRT::Utils::IsWinRtWrapperOf<::Windows::Foundation::Collections::IVector<T>^>(args.This()))
				{
					return scope.Close(Undefined());
				}

				VectorWrapper<T>* wrapper = VectorWrapper<T>::Unwrap<VectorWrapper<T>>(args.This());

				if (args.Length() == 1 && NodeRT::Utils::IsWinRtWrapperOf<::Platform::Array<T>^>(args[0]))
				{
					try
					{
						WrapperBase* itemsWrapper = WrapperBase::Unwrap<WrapperBase>(args[0].As<Object>());
						::Platform::Array<T>^ items = (::Platform::Array<T>^)itemsWrapper->GetObjectInstance();
						wrapper->_instance->ReplaceAll(items);
						return scope.Close(Undefined());
					}
					catch (Platform::Exception ^exception)
					{
						NodeRT::Utils::ThrowWinRtExceptionInJs(exception);
						return scope.Close(Undefined());
					}
				}
				else
				{
					ThrowException(Exception::Error(NodeRT::Utils::NewString(L"Bad arguments: no suitable overload found")));
					return scope.Close(Undefined());
				}
			}

			static Handle<Value> GetAt(const v8::Arguments& args)
			{
				HandleScope scope;

				if (!NodeRT::Utils::IsWinRtWrapperOf<::Windows::Foundation::Collections::IVector<T>^>(args.This()))
				{
					return scope.Close(Undefined());
				}

				VectorWrapper<T>* wrapper = VectorWrapper<T>::Unwrap<VectorWrapper<T>>(args.This());

				if (args.Length() == 1 && args[0]->IsUint32())
				{
					try
					{
						unsigned int index = args[0]->Uint32Value();

						if (index >= wrapper->_instance->Size)
						{
							return scope.Close(Undefined());
						}
						T result;
						result = wrapper->_instance->GetAt(index);

						if (wrapper->_getterFunc)
						{
							return scope.Close(wrapper->_getterFunc(result));
						}
					}
					catch (Platform::Exception ^exception)
					{
						NodeRT::Utils::ThrowWinRtExceptionInJs(exception);
						return scope.Close(Undefined());
					}
				}
				else
				{
					ThrowException(Exception::Error(NodeRT::Utils::NewString(L"Bad arguments: no suitable overload found")));
					return scope.Close(Undefined());
				}

				return scope.Close(Undefined());
			}

			static Handle<Value> SetAt(const v8::Arguments& args)
			{
				HandleScope scope;

				if (!NodeRT::Utils::IsWinRtWrapperOf<::Windows::Foundation::Collections::IVector<T>^>(args.This()))
				{
					return scope.Close(Undefined());
				}

				VectorWrapper<T>* wrapper = VectorWrapper<T>::Unwrap<VectorWrapper<T>>(args.This());

				if (args.Length() == 2 && args[0]->IsUint32() && wrapper->_checkTypeFunc(args[1]))
				{
					try
					{
						unsigned int index = args[0]->Uint32Value();

						if (index >= wrapper->_instance->Size)
						{
							return scope.Close(Undefined());
						}

						T item = wrapper->_convertToTypeFunc(args[1]);

						wrapper->_instance->SetAt(index, item);
					}
					catch (Platform::Exception ^exception)
					{
						NodeRT::Utils::ThrowWinRtExceptionInJs(exception);
						return scope.Close(Undefined());
					}
				}
				else
				{
					ThrowException(Exception::Error(NodeRT::Utils::NewString(L"Bad arguments: no suitable overload found")));
					return scope.Close(Undefined());
				}

				return scope.Close(Undefined());
			}


			static Handle<Value> First(const v8::Arguments& args)
			{
				HandleScope scope;

				if (!NodeRT::Utils::IsWinRtWrapperOf<::Windows::Foundation::Collections::IVector<T>^>(args.This()))
				{
					return scope.Close(Undefined());
				}

				VectorWrapper<T>* wrapper = VectorWrapper<T>::Unwrap<VectorWrapper<T>>(args.This());

				if (args.Length() == 0)
				{
					try
					{
						return IteratorWrapper<T>::CreateIteratorWrapper(wrapper->_instance->First(), wrapper->_getterFunc);
					}
					catch (Platform::Exception ^exception)
					{
						NodeRT::Utils::ThrowWinRtExceptionInJs(exception);
						return scope.Close(Undefined());
					}
				}
				else
				{
					ThrowException(Exception::Error(NodeRT::Utils::NewString(L"Bad arguments: no suitable overload found")));
					return scope.Close(Undefined());
				}

				return scope.Close(Undefined());
			}


			static Handle<Value> IndexOf(const v8::Arguments& args)
			{
				HandleScope scope;

				if (!NodeRT::Utils::IsWinRtWrapperOf<::Windows::Foundation::Collections::IVector<T>^>(args.This()))
				{
					return scope.Close(Undefined());
				}

				VectorWrapper<T>* wrapper = VectorWrapper<T>::Unwrap<VectorWrapper<T>>(args.This());

				if (wrapper->_convertToTypeFunc == nullptr || wrapper->_checkTypeFunc == nullptr)
				{
					ThrowException(Exception::Error(NodeRT::Utils::NewString(L"Method isn't supported")));
					return scope.Close(Undefined());
				}

				if (args.Length() == 1 && wrapper->_checkTypeFunc(args[0]))
				{
					try
					{
						T item = wrapper->_convertToTypeFunc(args[0]);

						unsigned int index;
						bool result = wrapper->_instance->IndexOf(item, &index);

						Handle<Object> resObj = Object::New();
						resObj->Set(String::NewSymbol("boolean"), Boolean::New(result));
						resObj->Set(String::NewSymbol("index"), Integer::NewFromUnsigned(index));
						return scope.Close(resObj);
					}
					catch (Platform::Exception ^exception)
					{
						NodeRT::Utils::ThrowWinRtExceptionInJs(exception);
						return scope.Close(Undefined());
					}
				}
				else
				{
					ThrowException(Exception::Error(NodeRT::Utils::NewString(L"Bad arguments: no suitable overload found")));
					return scope.Close(Undefined());
				}

				return scope.Close(Undefined());
			}

			static Handle<Value> SizeGetter(Local<String> property, const AccessorInfo &info)
			{
				HandleScope scope;
				if (!NodeRT::Utils::IsWinRtWrapperOf<::Windows::Foundation::Collections::IVector<T>^>(info.This()))
				{
					return scope.Close(Undefined());
				}

				VectorWrapper<T>* wrapper = VectorWrapper<T>::Unwrap<VectorWrapper<T>>(info.This());

				try
				{
					return Integer::NewFromUnsigned(wrapper->_instance->Size);
				}
				catch (Platform::Exception ^exception)
				{
					NodeRT::Utils::ThrowWinRtExceptionInJs(exception);
					return scope.Close(Undefined());
				}
			}

		private:
			::Windows::Foundation::Collections::IVector<T>^ _instance;
			std::function<Handle<Value>(T)> _getterFunc;
			std::function<bool(Handle<Value>)> _checkTypeFunc;
			std::function<T(Handle<Value>)> _convertToTypeFunc;
			static Persistent<FunctionTemplate> s_constructorTemplate;
		};

		template <class T>
		Persistent<FunctionTemplate> VectorWrapper<T>::s_constructorTemplate;

		template <class K, class V>
		class KeyValuePairWrapper : NodeRT::WrapperBase
		{
		public:

			static v8::Handle<v8::Value> Init()
			{
				HandleScope scope;

				s_constructorTemplate = Persistent<FunctionTemplate>::New(FunctionTemplate::New(New));
				s_constructorTemplate->SetClassName(String::NewSymbol("Windows::Foundation::Collections:IKeyValuePair"));
				s_constructorTemplate->InstanceTemplate()->SetInternalFieldCount(1);

				s_constructorTemplate->PrototypeTemplate()->SetAccessor(String::NewSymbol("key"), KeyGetter);
				s_constructorTemplate->PrototypeTemplate()->SetAccessor(String::NewSymbol("value"), ValueGetter);

				return scope.Close(Undefined());
			}

			static Handle<Value> CreateKeyValuePairWrapper(::Windows::Foundation::Collections::IKeyValuePair<K, V>^ winRtInstance,
				std::function<Handle<Value>(K)> keyGetterFunc,
				std::function<Handle<Value>(V)> valueGetterFunc)
			{
				HandleScope scope;
				if (winRtInstance == nullptr)
				{
					return scope.Close(Undefined());
				}

				if (s_constructorTemplate.IsEmpty())
				{
					Init();
				}

				v8::Handle<Value> args [] = { v8::Undefined() };

				v8::Handle<v8::Object> objectInstance = s_constructorTemplate->GetFunction()->NewInstance(0, args);
				if (objectInstance.IsEmpty())
				{
					return scope.Close(Undefined());
				}

				KeyValuePairWrapper<K, V> *wrapperInstance = new KeyValuePairWrapper<K, V>(winRtInstance, keyGetterFunc, valueGetterFunc);
				wrapperInstance->Wrap(objectInstance);
				return scope.Close(objectInstance);
			}

			virtual ::Platform::Object^ GetObjectInstance() const override
			{
				return _instance;
			}

		private:

			KeyValuePairWrapper(::Windows::Foundation::Collections::IKeyValuePair<K, V>^ winRtInstance,
				std::function<Handle<Value>(K)> keyGetterFunc,
				std::function<Handle<Value>(V)> valueGetterFunc) :
				_instance(winRtInstance),
				_keyGetterFunc(keyGetterFunc),
				_valueGetterFunc(valueGetterFunc)
			{

			}

			static v8::Handle<v8::Value> New(const v8::Arguments& args)
			{
				args.This()->SetHiddenValue(String::NewSymbol("__winRtInstance__"), True());

				return args.This();
			}

			static Handle<Value> KeyGetter(Local<String> property, const AccessorInfo &info)
			{
				HandleScope scope;
				if (!NodeRT::Utils::IsWinRtWrapperOf<::Windows::Foundation::Collections::IKeyValuePair<K, V>^>(info.This()))
				{
					return scope.Close(Undefined());
				}

				KeyValuePairWrapper<K, V>* wrapper = KeyValuePairWrapper<K, V>::Unwrap<KeyValuePairWrapper<K, V>>(info.This());

				try
				{
					return scope.Close(wrapper->_keyGetterFunc(wrapper->_instance->Key));
				}
				catch (Platform::Exception ^exception)
				{
					NodeRT::Utils::ThrowWinRtExceptionInJs(exception);
					return scope.Close(Undefined());
				}
			}

			static Handle<Value> ValueGetter(Local<String> property, const AccessorInfo &info)
			{
				HandleScope scope;
				if (!NodeRT::Utils::IsWinRtWrapperOf<::Windows::Foundation::Collections::IKeyValuePair<K, V>^>(info.This()))
				{
					return scope.Close(Undefined());
				}

				KeyValuePairWrapper<K, V>* wrapper = KeyValuePairWrapper<K, V>::Unwrap<KeyValuePairWrapper<K, V>>(info.This());

				try
				{
					return scope.Close(wrapper->_valueGetterFunc(wrapper->_instance->Value));
				}
				catch (Platform::Exception ^exception)
				{
					NodeRT::Utils::ThrowWinRtExceptionInJs(exception);
					return scope.Close(Undefined());
				}
			}

		private:
			::Windows::Foundation::Collections::IKeyValuePair<K, V>^ _instance;
			std::function<Handle<Value>(K)> _keyGetterFunc;
			std::function<Handle<Value>(V)> _valueGetterFunc;
			static Persistent<FunctionTemplate> s_constructorTemplate;
		};

		template <class K, class V>
		Persistent<FunctionTemplate> KeyValuePairWrapper<K, V>::s_constructorTemplate;

		template <class K, class V>
		class MapViewWrapper : NodeRT::WrapperBase
		{
		public:

			static v8::Handle<v8::Value> Init()
			{
				HandleScope scope;

				s_constructorTemplate = Persistent<FunctionTemplate>::New(FunctionTemplate::New(New));
				s_constructorTemplate->SetClassName(String::NewSymbol("Windows::Foundation::Collections:IMapView"));
				s_constructorTemplate->InstanceTemplate()->SetInternalFieldCount(1);

				s_constructorTemplate->PrototypeTemplate()->Set(String::NewSymbol("hasKey"), FunctionTemplate::New(HasKey)->GetFunction());
				s_constructorTemplate->PrototypeTemplate()->Set(String::NewSymbol("lookup"), FunctionTemplate::New(Lookup)->GetFunction());
				s_constructorTemplate->PrototypeTemplate()->Set(String::NewSymbol("split"), FunctionTemplate::New(Split)->GetFunction());
				s_constructorTemplate->PrototypeTemplate()->Set(String::NewSymbol("first"), FunctionTemplate::New(First)->GetFunction());

				s_constructorTemplate->PrototypeTemplate()->SetAccessor(String::NewSymbol("size"), SizeGetter);
				s_constructorTemplate->PrototypeTemplate()->SetAccessor(String::NewSymbol("length"), SizeGetter);

				return scope.Close(Undefined());
			}

			static Handle<Value> CreateMapViewWrapper(::Windows::Foundation::Collections::IMapView<K, V>^ winRtInstance,
				std::function<Handle<Value>(K)> keyGetterFunc,
				std::function<bool(Handle<Value>)> checkKeyTypeFunc,
				std::function<K(Handle<Value>)> convertToKeyTypeFunc,
				std::function<Handle<Value>(V)> valueGetterFunc)
			{
				HandleScope scope;
				if (winRtInstance == nullptr)
				{
					return scope.Close(Undefined());
				}

				if (s_constructorTemplate.IsEmpty())
				{
					Init();
				}

				v8::Handle<Value> args [] = { v8::Undefined() };

				v8::Handle<v8::Object> objectInstance = s_constructorTemplate->GetFunction()->NewInstance(0, args);
				if (objectInstance.IsEmpty())
				{
					return scope.Close(Undefined());
				}

				MapViewWrapper<K, V> *wrapperInstance = new MapViewWrapper<K, V>(winRtInstance, keyGetterFunc, checkKeyTypeFunc, convertToKeyTypeFunc, valueGetterFunc);
				wrapperInstance->Wrap(objectInstance);
				return scope.Close(objectInstance);
			}

			virtual ::Platform::Object^ GetObjectInstance() const override
			{
				return _instance;
			}

		private:

			MapViewWrapper(::Windows::Foundation::Collections::IMapView<K, V>^ winRtInstance,
				std::function<Handle<Value>(K)> keyGetterFunc,
				std::function<bool(Handle<Value>)> checkKeyTypeFunc,
				std::function<K(Handle<Value>)> convertToKeyTypeFunc,
				std::function<Handle<Value>(V)> valueGetterFunc) :
				_instance(winRtInstance),
				_keyGetterFunc(keyGetterFunc),
				_checkKeyTypeFunc(checkKeyTypeFunc),
				_convertToKeyTypeFunc(convertToKeyTypeFunc),
				_valueGetterFunc(valueGetterFunc)
			{

			}

			static v8::Handle<v8::Value> New(const v8::Arguments& args)
			{
				args.This()->SetHiddenValue(String::NewSymbol("__winRtInstance__"), True());

				return args.This();
			}


			static Handle<Value> HasKey(const v8::Arguments& args)
			{
				HandleScope scope;

				if (!NodeRT::Utils::IsWinRtWrapperOf<::Windows::Foundation::Collections::IMapView<K, V>^>(args.This()))
				{
					return scope.Close(Undefined());
				}

				MapViewWrapper<K, V>* wrapper = MapViewWrapper<K, V>::Unwrap<MapViewWrapper<K, V>>(args.This());

				if (args.Length() == 1 && wrapper->_checkKeyTypeFunc(args[0]))
				{
					try
					{
						K key = wrapper->_convertToKeyTypeFunc(args[0]);

						bool result = wrapper->_instance->HasKey(key);

						return scope.Close(Boolean::New(result));
					}
					catch (Platform::Exception ^exception)
					{
						NodeRT::Utils::ThrowWinRtExceptionInJs(exception);
						return scope.Close(Undefined());
					}
				}
				else
				{
					ThrowException(Exception::Error(NodeRT::Utils::NewString(L"Bad arguments: no suitable overload found")));
					return scope.Close(Undefined());
				}

				return scope.Close(Undefined());
			}


			static Handle<Value> First(const v8::Arguments& args)
			{
				HandleScope scope;

				if (!NodeRT::Utils::IsWinRtWrapperOf<::Windows::Foundation::Collections::IMapView<K, V>^>(args.This()))
				{
					return scope.Close(Undefined());
				}

				MapViewWrapper<K, V>* wrapper = MapViewWrapper<K, V>::Unwrap<MapViewWrapper<K, V>>(args.This());

				if (args.Length() == 0)
				{
					try
					{
						std::function<Handle<Value>(K)> keyGetter = wrapper->_keyGetterFunc;
						std::function<Handle<Value>(V)> valueGetter = wrapper->_valueGetterFunc;
						return scope.Close(IteratorWrapper<::Windows::Foundation::Collections::IKeyValuePair<K, V>^>::CreateIteratorWrapper(wrapper->_instance->First(),
							[keyGetter, valueGetter](::Windows::Foundation::Collections::IKeyValuePair<K, V>^ value) {
							return KeyValuePairWrapper<K, V>::CreateKeyValuePairWrapper(value,
								keyGetter,
								valueGetter);
						}));
					}
					catch (Platform::Exception ^exception)
					{
						NodeRT::Utils::ThrowWinRtExceptionInJs(exception);
						return scope.Close(Undefined());
					}
				}
				else
				{
					ThrowException(Exception::Error(NodeRT::Utils::NewString(L"Bad arguments: no suitable overload found")));
					return scope.Close(Undefined());
				}

				return scope.Close(Undefined());
			}


			static Handle<Value> Lookup(const v8::Arguments& args)
			{
				HandleScope scope;

				if (!NodeRT::Utils::IsWinRtWrapperOf<::Windows::Foundation::Collections::IMapView<K, V>^>(args.This()))
				{
					return scope.Close(Undefined());
				}

				MapViewWrapper<K, V>* wrapper = MapViewWrapper<K, V>::Unwrap<MapViewWrapper<K, V>>(args.This());

				if (args.Length() == 1 && wrapper->_checkKeyTypeFunc(args[0]))
				{
					try
					{
						K key = wrapper->_convertToKeyTypeFunc(args[0]);

						V result = wrapper->_instance->Lookup(key);

						return scope.Close(wrapper->_valueGetterFunc(result));
					}
					catch (Platform::Exception ^exception)
					{
						NodeRT::Utils::ThrowWinRtExceptionInJs(exception);
						return scope.Close(Undefined());
					}
				}
				else
				{
					ThrowException(Exception::Error(NodeRT::Utils::NewString(L"Bad arguments: no suitable overload found")));
					return scope.Close(Undefined());
				}

				return scope.Close(Undefined());
			}



			static Handle<Value> Split(const v8::Arguments& args)
			{
				HandleScope scope;

				if (!NodeRT::Utils::IsWinRtWrapperOf<::Windows::Foundation::Collections::IMapView<K, V>^>(args.This()))
				{
					return scope.Close(Undefined());
				}

				MapViewWrapper<K, V>* wrapper = MapViewWrapper<K, V>::Unwrap<MapViewWrapper<K, V>>(args.This());

				if (args.Length() == 0)
				{
					try
					{
						::Windows::Foundation::Collections::IMapView<K, V> ^first;
						::Windows::Foundation::Collections::IMapView<K, V> ^second;

						wrapper->_instance->Split(&first, &second);

						Handle<Object> resObj = Object::New();
						resObj->Set(String::NewSymbol("first"), MapViewWrapper<K, V>::CreateMapViewWrapper(first, wrapper->_keyGetterFunc, wrapper->_checkTypeFunc, wrapper->_convertToKeyTypeFunc, wrapper->_valueGetterFunc));
						resObj->Set(String::NewSymbol("second"), MapViewWrapper<K, V>::CreateMapViewWrapper(second, wrapper->_keyGetterFunc, wrapper->_checkTypeFunc, wrapper->_convertToKeyTypeFunc, wrapper->_valueGetterFunc));
						return scope.Close(resObj);
					}
					catch (Platform::Exception ^exception)
					{
						NodeRT::Utils::ThrowWinRtExceptionInJs(exception);
						return scope.Close(Undefined());
					}
				}
				else
				{
					ThrowException(Exception::Error(NodeRT::Utils::NewString(L"Bad arguments: no suitable overload found")));
					return scope.Close(Undefined());
				}

				return scope.Close(Undefined());
			}

			static Handle<Value> SizeGetter(Local<String> property, const AccessorInfo &info)
			{
				HandleScope scope;
				if (!NodeRT::Utils::IsWinRtWrapperOf<::Windows::Foundation::Collections::IMapView<K, V>^>(info.This()))
				{
					return scope.Close(Undefined());
				}

				MapViewWrapper<K, V>* wrapper = MapViewWrapper<K, V>::Unwrap<MapViewWrapper<K, V>>(info.This());

				try
				{
					return Integer::NewFromUnsigned(wrapper->_instance->Size);
				}
				catch (Platform::Exception ^exception)
				{
					NodeRT::Utils::ThrowWinRtExceptionInJs(exception);
					return scope.Close(Undefined());
				}
			}

		private:
			::Windows::Foundation::Collections::IMapView<K, V>^ _instance;
			std::function<bool(Handle<Value>)> _checkTypeFunc;
			std::function<Handle<Value>(K)> _keyGetterFunc;
			std::function<K(Handle<Value>)> _convertToKeyTypeFunc;
			std::function<Handle<Value>(V)> _valueGetterFunc;
			std::function<bool(Handle<Value>)> _checkKeyTypeFunc;
			static Persistent<FunctionTemplate> s_constructorTemplate;
		};

		template <class K, class V>
		Persistent<FunctionTemplate> MapViewWrapper<K, V>::s_constructorTemplate;

		template <class K, class V>
		class MapWrapper : NodeRT::WrapperBase
		{
		public:

			static v8::Handle<v8::Value> Init()
			{
				HandleScope scope;

				s_constructorTemplate = Persistent<FunctionTemplate>::New(FunctionTemplate::New(New));
				s_constructorTemplate->SetClassName(String::NewSymbol("Windows::Foundation::Collections:IMap"));
				s_constructorTemplate->InstanceTemplate()->SetInternalFieldCount(1);

				s_constructorTemplate->PrototypeTemplate()->Set(String::NewSymbol("hasKey"), FunctionTemplate::New(HasKey)->GetFunction());
				s_constructorTemplate->PrototypeTemplate()->Set(String::NewSymbol("lookup"), FunctionTemplate::New(Lookup)->GetFunction());
				s_constructorTemplate->PrototypeTemplate()->Set(String::NewSymbol("getView"), FunctionTemplate::New(GetView)->GetFunction());
				s_constructorTemplate->PrototypeTemplate()->Set(String::NewSymbol("clear"), FunctionTemplate::New(Clear)->GetFunction());
				s_constructorTemplate->PrototypeTemplate()->Set(String::NewSymbol("insert"), FunctionTemplate::New(Insert)->GetFunction());
				s_constructorTemplate->PrototypeTemplate()->Set(String::NewSymbol("remove"), FunctionTemplate::New(Remove)->GetFunction());
				s_constructorTemplate->PrototypeTemplate()->Set(String::NewSymbol("first"), FunctionTemplate::New(First)->GetFunction());

				s_constructorTemplate->PrototypeTemplate()->SetAccessor(String::NewSymbol("size"), SizeGetter);
				s_constructorTemplate->PrototypeTemplate()->SetAccessor(String::NewSymbol("length"), SizeGetter);

				return scope.Close(Undefined());
			}

			static Handle<Value> CreateMapWrapper(::Windows::Foundation::Collections::IMap<K, V>^ winRtInstance,
				std::function<Handle<Value>(K)> keyGetterFunc,
				std::function<bool(Handle<Value>)> checkKeyTypeFunc,
				std::function<K(Handle<Value>)> convertToKeyTypeFunc,
				std::function<Handle<Value>(V)> valueGetterFunc,
				std::function<bool(Handle<Value>)> checkValueTypeFunc,
				std::function<V(Handle<Value>)> convertToValueTypeFunc)
			{
				HandleScope scope;
				if (winRtInstance == nullptr)
				{
					return scope.Close(Undefined());
				}

				if (s_constructorTemplate.IsEmpty())
				{
					Init();
				}

				v8::Handle<Value> args [] = { v8::Undefined() };

				v8::Handle<v8::Object> objectInstance = s_constructorTemplate->GetFunction()->NewInstance(0, args);
				if (objectInstance.IsEmpty())
				{
					return scope.Close(Undefined());
				}

				MapWrapper<K, V> *wrapperInstance = new MapWrapper<K, V>(winRtInstance,
					keyGetterFunc,
					checkKeyTypeFunc,
					convertToKeyTypeFunc,
					valueGetterFunc,
					checkValueTypeFunc,
					convertToValueTypeFunc);
				wrapperInstance->Wrap(objectInstance);
				return scope.Close(objectInstance);
			}

			virtual ::Platform::Object^ GetObjectInstance() const override
			{
				return _instance;
			}

		private:

			MapWrapper(::Windows::Foundation::Collections::IMap<K, V>^ winRtInstance,
				std::function<Handle<Value>(K)> keyGetterFunc,
				std::function<bool(Handle<Value>)> checkKeyTypeFunc,
				std::function<K(Handle<Value>)> convertToKeyTypeFunc,
				std::function<Handle<Value>(V)> valueGetterFunc,
				std::function<bool(Handle<Value>)> checkValueTypeFunc,
				std::function<V(Handle<Value>)> convertToValueTypeFunc) :
				_instance(winRtInstance),
				_keyGetterFunc(keyGetterFunc),
				_checkKeyTypeFunc(checkKeyTypeFunc),
				_convertToKeyTypeFunc(convertToKeyTypeFunc),
				_valueGetterFunc(valueGetterFunc),
				_checkValueTypeFunc(checkValueTypeFunc),
				_convertToValueTypeFunc(convertToValueTypeFunc)
			{

			}

			static v8::Handle<v8::Value> New(const v8::Arguments& args)
			{
				args.This()->SetHiddenValue(String::NewSymbol("__winRtInstance__"), True());

				return args.This();
			}

			static Handle<Value> HasKey(const v8::Arguments& args)
			{
				HandleScope scope;

				if (!NodeRT::Utils::IsWinRtWrapperOf<::Windows::Foundation::Collections::IMap<K, V>^>(args.This()))
				{
					return scope.Close(Undefined());
				}

				MapWrapper<K, V>* wrapper = MapWrapper<K, V>::Unwrap<MapWrapper<K, V>>(args.This());

				if (args.Length() == 1 && wrapper->_checkKeyTypeFunc(args[0]))
				{
					try
					{
						K key = wrapper->_convertToKeyTypeFunc(args[0]);

						bool result = wrapper->_instance->HasKey(key);

						return scope.Close(Boolean::New(result));
					}
					catch (Platform::Exception ^exception)
					{
						NodeRT::Utils::ThrowWinRtExceptionInJs(exception);
						return scope.Close(Undefined());
					}
				}
				else
				{
					ThrowException(Exception::Error(NodeRT::Utils::NewString(L"Bad arguments: no suitable overload found")));
					return scope.Close(Undefined());
				}

				return scope.Close(Undefined());
			}

			static Handle<Value> Remove(const v8::Arguments& args)
			{
				HandleScope scope;

				if (!NodeRT::Utils::IsWinRtWrapperOf<::Windows::Foundation::Collections::IMap<K, V>^>(args.This()))
				{
					return scope.Close(Undefined());
				}

				MapWrapper<K, V>* wrapper = MapWrapper<K, V>::Unwrap<MapWrapper<K, V>>(args.This());

				if (args.Length() == 1 && wrapper->_checkKeyTypeFunc(args[0]))
				{
					try
					{
						K key = wrapper->_convertToKeyTypeFunc(args[0]);

						wrapper->_instance->Remove(key);

						return scope.Close(Undefined());
					}
					catch (Platform::Exception ^exception)
					{
						NodeRT::Utils::ThrowWinRtExceptionInJs(exception);
						return scope.Close(Undefined());
					}
				}
				else
				{
					ThrowException(Exception::Error(NodeRT::Utils::NewString(L"Bad arguments: no suitable overload found")));
					return scope.Close(Undefined());
				}
			}

			static Handle<Value> Insert(const v8::Arguments& args)
			{
				HandleScope scope;

				if (!NodeRT::Utils::IsWinRtWrapperOf<::Windows::Foundation::Collections::IMap<K, V>^>(args.This()))
				{
					return scope.Close(Undefined());
				}

				MapWrapper<K, V>* wrapper = MapWrapper<K, V>::Unwrap<MapWrapper<K, V>>(args.This());

				if (args.Length() == 2 && wrapper->_checkKeyTypeFunc(args[0]) && wrapper->_checkValueTypeFunc(args[1]))
				{
					try
					{
						K key = wrapper->_convertToKeyTypeFunc(args[0]);
						V value = wrapper->_convertToValueTypeFunc(args[1]);

						bool result = wrapper->_instance->Insert(key, value);

						return scope.Close(Boolean::New(result));
					}
					catch (Platform::Exception ^exception)
					{
						NodeRT::Utils::ThrowWinRtExceptionInJs(exception);
						return scope.Close(Undefined());
					}
				}
				else
				{
					ThrowException(Exception::Error(NodeRT::Utils::NewString(L"Bad arguments: no suitable overload found")));
					return scope.Close(Undefined());
				}
			}

			static Handle<Value> First(const v8::Arguments& args)
			{
				HandleScope scope;

				if (!NodeRT::Utils::IsWinRtWrapperOf<::Windows::Foundation::Collections::IMap<K, V>^>(args.This()))
				{
					return scope.Close(Undefined());
				}

				MapWrapper<K, V>* wrapper = MapWrapper<K, V>::Unwrap<MapWrapper<K, V>>(args.This());

				if (args.Length() == 0)
				{
					try
					{
						std::function<Handle<Value>(K)> keyGetter = wrapper->_keyGetterFunc;
						std::function<Handle<Value>(V)> valueGetter = wrapper->_valueGetterFunc;
						return scope.Close(IteratorWrapper<::Windows::Foundation::Collections::IKeyValuePair<K, V>^>::CreateIteratorWrapper(wrapper->_instance->First(),
							[keyGetter, valueGetter](::Windows::Foundation::Collections::IKeyValuePair<K, V>^ value) {
							return KeyValuePairWrapper<K, V>::CreateKeyValuePairWrapper(value,
								keyGetter,
								valueGetter);
						}));
					}
					catch (Platform::Exception ^exception)
					{
						NodeRT::Utils::ThrowWinRtExceptionInJs(exception);
						return scope.Close(Undefined());
					}
				}
				else
				{
					ThrowException(Exception::Error(NodeRT::Utils::NewString(L"Bad arguments: no suitable overload found")));
					return scope.Close(Undefined());
				}

				return scope.Close(Undefined());
			}

			static Handle<Value> Lookup(const v8::Arguments& args)
			{
				HandleScope scope;

				if (!NodeRT::Utils::IsWinRtWrapperOf<::Windows::Foundation::Collections::IMap<K, V>^>(args.This()))
				{
					return scope.Close(Undefined());
				}

				MapWrapper<K, V>* wrapper = MapWrapper<K, V>::Unwrap<MapWrapper<K, V>>(args.This());

				if (args.Length() == 1 && wrapper->_checkKeyTypeFunc(args[0]))
				{
					try
					{
						K key = wrapper->_convertToKeyTypeFunc(args[0]);

						V result = wrapper->_instance->Lookup(key);

						return scope.Close(wrapper->_valueGetterFunc(result));
					}
					catch (Platform::Exception ^exception)
					{
						NodeRT::Utils::ThrowWinRtExceptionInJs(exception);
						return scope.Close(Undefined());
					}
				}
				else
				{
					ThrowException(Exception::Error(NodeRT::Utils::NewString(L"Bad arguments: no suitable overload found")));
					return scope.Close(Undefined());
				}

				return scope.Close(Undefined());
			}

			static Handle<Value> GetView(const v8::Arguments& args)
			{
				HandleScope scope;

				if (!NodeRT::Utils::IsWinRtWrapperOf<::Windows::Foundation::Collections::IMap<K, V>^>(args.This()))
				{
					return scope.Close(Undefined());
				}

				MapWrapper<K, V>* wrapper = MapWrapper<K, V>::Unwrap<MapWrapper<K, V>>(args.This());

				if (args.Length() == 0)
				{
					try
					{
						::Windows::Foundation::Collections::IMapView<K, V>^ result = wrapper->_instance->GetView();

						return scope.Close(MapViewWrapper<K, V>::CreateMapViewWrapper(result,
							wrapper->_keyGetterFunc,
							wrapper->_checkKeyTypeFunc,
							wrapper->_convertToKeyTypeFunc,
							wrapper->_valueGetterFunc));
					}
					catch (Platform::Exception ^exception)
					{
						NodeRT::Utils::ThrowWinRtExceptionInJs(exception);
						return scope.Close(Undefined());
					}
				}
				else
				{
					ThrowException(Exception::Error(NodeRT::Utils::NewString(L"Bad arguments: no suitable overload found")));
					return scope.Close(Undefined());
				}
			}

			static Handle<Value> Clear(const v8::Arguments& args)
			{
				HandleScope scope;

				if (!NodeRT::Utils::IsWinRtWrapperOf<::Windows::Foundation::Collections::IMap<K, V>^>(args.This()))
				{
					return scope.Close(Undefined());
				}

				MapWrapper<K, V>* wrapper = MapWrapper<K, V>::Unwrap<MapWrapper<K, V>>(args.This());

				if (args.Length() == 0)
				{
					try
					{
						wrapper->_instance->Clear();
						return scope.Close(Undefined());
					}
					catch (Platform::Exception ^exception)
					{
						NodeRT::Utils::ThrowWinRtExceptionInJs(exception);
						return scope.Close(Undefined());
					}
				}
				else
				{
					ThrowException(Exception::Error(NodeRT::Utils::NewString(L"Bad arguments: no suitable overload found")));
					return scope.Close(Undefined());
				}
			}

			static Handle<Value> SizeGetter(Local<String> property, const AccessorInfo &info)
			{
				HandleScope scope;
				if (!NodeRT::Utils::IsWinRtWrapperOf<::Windows::Foundation::Collections::IMap<K, V>^>(info.This()))
				{
					return scope.Close(Undefined());
				}

				MapWrapper<K, V>* wrapper = MapWrapper<K, V>::Unwrap<MapWrapper<K, V>>(info.This());

				try
				{
					return Integer::NewFromUnsigned(wrapper->_instance->Size);
				}
				catch (Platform::Exception ^exception)
				{
					NodeRT::Utils::ThrowWinRtExceptionInJs(exception);
					return scope.Close(Undefined());
				}
			}

		private:
			::Windows::Foundation::Collections::IMap<K, V>^ _instance;

			std::function<Handle<Value>(K)> _keyGetterFunc;
			std::function<K(Handle<Value>)> _convertToKeyTypeFunc;
			std::function<bool(Handle<Value>)> _checkKeyTypeFunc;

			std::function<Handle<Value>(V)> _valueGetterFunc;
			std::function<V(Handle<Value>)> _convertToValueTypeFunc;
			std::function<bool(Handle<Value>)> _checkValueTypeFunc;

			static Persistent<FunctionTemplate> s_constructorTemplate;
		};

		template <class K, class V>
		Persistent<FunctionTemplate> MapWrapper<K, V>::s_constructorTemplate;

	}
};