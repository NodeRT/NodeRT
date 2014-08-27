    static v8::Handle<v8::Value> AddListener(const v8::Arguments& args)
    {
      HandleScope scope;

      if (args.Length() < 2 || !args[0]->IsString() || !args[1]->IsFunction())
      {
        ThrowException(Exception::Error(NodeRT::Utils::NewString(L"wrong arguments, expected arguments are eventName(string),callback(function)")));
        return scope.Close(Undefined());
      }

      String::Value eventName(args[0]);
      auto str = *eventName;
      
      Local<Function> callback = args[1].As<Function>();
      
      ::Windows::Foundation::EventRegistrationToken registrationToken;
      if (NodeRT::Utils::CaseInsenstiveEquals(L"@TX.Uncap(Model.Events[0].EventInfo.Name)", str))
      {
        @if (!Model.Events[0].IsStatic)
        {
        @:if (!NodeRT::Utils::IsWinRtWrapperOf<@(TX.ToWinRT(Model.Type,true))>(args.This()))
        @:{
        @:  ThrowException(Exception::Error(NodeRT::Utils::NewString(L"The caller of this method isn't of the expected type or internal WinRt object was disposed")));
        @:  return scope.Close(Undefined());
        @:}
        @:@(Model.Name) *wrapper = @(Model.Name)::Unwrap<@(Model.Name)>(args.This());
        }
      @TX.CppTemplates.RegisterEventWithWinRT(Model.Events[0])
      }
      @for(var i=1; i<Model.Events.Length; i++) {
      @:else if (NodeRT::Utils::CaseInsenstiveEquals(L"@TX.Uncap(Model.Events[i].EventInfo.Name)", str))
      @:{
        @if (!Model.Events[i].IsStatic)
        {
        @:if (!NodeRT::Utils::IsWinRtWrapperOf<@(TX.ToWinRT(Model.Type,true))>(args.This()))
        @:{
        @:  ThrowException(Exception::Error(NodeRT::Utils::NewString(L"The caller of this method isn't of the expected type or internal WinRt object was disposed")));
        @:  return scope.Close(Undefined());
        @:}
        @:@(Model.Name) *wrapper = @(Model.Name)::Unwrap<@(Model.Name)>(args.This());
        }
      @:@TX.CppTemplates.RegisterEventWithWinRT(Model.Events[i])
      @:}
      }
      else 
      {
        ThrowException(Exception::Error(String::Concat(NodeRT::Utils::NewString(L"given event name isn't supported: "), args[0].As<String>())));
        return scope.Close(Undefined());
      }

      Local<Value> tokenMap = callback->GetHiddenValue(String::NewSymbol(REGISTRATION_TOKEN_MAP_PROPERTY_NAME));
                
      if (tokenMap.IsEmpty() || tokenMap->Equals(Undefined()))
      {
          tokenMap = Object::New();
          callback->SetHiddenValue(String::NewSymbol(REGISTRATION_TOKEN_MAP_PROPERTY_NAME), tokenMap);
      }

      tokenMap.As<Object>()->Set(args[1], CreateOpaqueWrapper(::Windows::Foundation::PropertyValue::CreateInt64(registrationToken.Value)));
                
      return scope.Close(Undefined());
    }

    static v8::Handle<v8::Value> RemoveListener(const v8::Arguments& args)
    {
      HandleScope scope;

      if (args.Length() < 2 || !args[0]->IsString() || !args[1]->IsFunction())
      {
        ThrowException(Exception::Error(NodeRT::Utils::NewString(L"wrong arguments, expected a string and a callback")));
        return scope.Close(Undefined());
      }

      String::Value eventName(args[0]);
      auto str = *eventName;

      if (@TX.ForEachEvent(Model.Events ,"(NodeRT::Utils::CaseInsenstiveEquals(L\"{1}\", str)) &&", 3))
      {
        ThrowException(Exception::Error(String::Concat(NodeRT::Utils::NewString(L"given event name isn't supported: "), args[0].As<String>())));
        return scope.Close(Undefined());
      }

      Local<Function> callback = args[1].As<Function>();
      Handle<Value> tokenMap = callback->GetHiddenValue(String::NewSymbol(REGISTRATION_TOKEN_MAP_PROPERTY_NAME));
                
      if (tokenMap.IsEmpty() || tokenMap->Equals(Undefined()))
      {
        return scope.Close(Undefined());
      }

      Handle<Value> opaqueWrapperObj =  tokenMap.As<Object>()->Get(args[1]);

      if (opaqueWrapperObj.IsEmpty() || opaqueWrapperObj->Equals(Undefined()))
      {
        return scope.Close(Undefined());
      }

      OpaqueWrapper *opaqueWrapper = OpaqueWrapper::Unwrap<OpaqueWrapper>(opaqueWrapperObj.As<Object>());
            
      long long tokenValue = (long long) opaqueWrapper->GetObjectInstance();
      ::Windows::Foundation::EventRegistrationToken registrationToken;
      registrationToken.Value = tokenValue;
        
      try 
      {
        if (NodeRT::Utils::CaseInsenstiveEquals(L"@TX.Uncap(Model.Events[0].EventInfo.Name)", str))
        {
          @if (!Model.Events[0].IsStatic)
          {
          @:if (!NodeRT::Utils::IsWinRtWrapperOf<@(TX.ToWinRT(Model.Type,true))>(args.This()))
          @:{
          @:  ThrowException(Exception::Error(NodeRT::Utils::NewString(L"The caller of this method isn't of the expected type or internal WinRt object was disposed")));
          @:  return scope.Close(Undefined());
          @:}
          @:@(Model.Name) *wrapper = @(Model.Name)::Unwrap<@(Model.Name)>(args.This());
          @:wrapper->_instance->@(Model.Events[0].EventInfo.Name)::remove(registrationToken);
          }
          else
          {
          @:@(TX.ToWinRT(Model.Events[0].EventInfo.DeclaringType, false))::@(Model.Events[0].EventInfo.Name)::remove(registrationToken);
          }
        }
        @for(var i=1; i<Model.Events.Length; i++) {
        @:else if (NodeRT::Utils::CaseInsenstiveEquals(L"@TX.Uncap(Model.Events[i].EventInfo.Name)", str))
        @:{
          @if (!Model.Events[i].IsStatic)
          {
          @:if (!NodeRT::Utils::IsWinRtWrapperOf<@(TX.ToWinRT(Model.Type,true))>(args.This()))
          @:{
          @:  ThrowException(Exception::Error(NodeRT::Utils::NewString(L"The caller of this method isn't of the expected type or internal WinRt object was disposed")));
          @:  return scope.Close(Undefined());
          @:}
          @:@(Model.Name) *wrapper = @(Model.Name)::Unwrap<@(Model.Name)>(args.This());
          @:wrapper->_instance->@(Model.Events[i].EventInfo.Name)::remove(registrationToken);
          }
          else
          {
          @:@(TX.ToWinRT(Model.Events[i].EventInfo.DeclaringType, false))::@(Model.Events[i].EventInfo.Name)::remove(registrationToken);
          }
        @:}
        }
      }
      catch (Platform::Exception ^exception)
      {
        NodeRT::Utils::ThrowWinRtExceptionInJs(exception);
      }

      tokenMap.As<Object>()->Delete(args[0].As<String>());

      return scope.Close(Undefined());
    }
