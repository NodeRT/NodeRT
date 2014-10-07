
    static v8::Handle<v8::Value> New(const v8::Arguments& args)
    {
      HandleScope scope;

      // in case the constructor was called without the new operator
      if (!s_constructorTemplate->HasInstance(args.This()))
      {
        if (args.Length() > 0)
        {
          std::unique_ptr<Handle<Value> []> constructorArgs(new Handle<Value>[args.Length()]);

          Handle<Value> *argsPtr = constructorArgs.get();
          for (int i = 0; i < args.Length(); i++)
          {
            argsPtr[i] = args[i];
          }

          return s_constructorTemplate->GetFunction()->CallAsConstructor(args.Length(), constructorArgs.get());
        }
        else
        {
          return s_constructorTemplate->GetFunction()->CallAsConstructor(args.Length(), nullptr);
        }
      }
      
      @TX.ToWinRT(Model.Type) winRtInstance;
      @{
          int c = 0;
          var constructors = Model.Type.GetConstructors();
      }

      if (args.Length() == 1 && OpaqueWrapper::IsOpaqueWrapper(args[0]) &&
        NodeRT::Utils::IsWinRtWrapperOf<@TX.ToWinRT(Model.Type)>(args[0]))
      {
        try 
        {
          winRtInstance = (@(TX.ToWinRT(Model.Type))) NodeRT::Utils::GetObjectInstance(args[0]);
        }
        catch (Platform::Exception ^exception)
        {
          NodeRT::Utils::ThrowWinRtExceptionInJs(exception);
          return scope.Close(Undefined());
        }
      }
      @foreach(var overload in constructors)
      {
        int i = 1;
      @:else if (args.Length() == @(overload.GetParameters().Length)@{if (overload.GetParameters().Length==0)@(")")}

        foreach (var paramInfo in overload.GetParameters())
        {
        
        @:&& @(String.Format(Converter.TypeCheck(paramInfo.ParameterType, TX.MainModel.Types.ContainsKey(paramInfo.ParameterType)), "args[" + (i-1).ToString() + "]"))@{if (overload.GetParameters().Length==i) @(")")}
          i++;
        }
      @:{
        @:try
        @:{
          int parameterCounter = 0;
          foreach (var paramInfo in overload.GetParameters())
          {
          var winrtConversionInfo = Converter.ToWinRT(paramInfo.ParameterType, TX.MainModel.Types.ContainsKey(paramInfo.ParameterType));   
          
          @:@(winrtConversionInfo[0]) arg@(parameterCounter) = @(string.Format(winrtConversionInfo[1], "args[" +parameterCounter + "]" ));
          parameterCounter++;
          }
          
          if (overload.GetParameters().Length > 0)
          {
          @:
          }
          @:winRtInstance = ref new @(TX.ToWinRT(Model.Type, false))(@{int j=0;foreach(var paramInfo in overload.GetParameters()){if(j>0)@(","); @("arg" + j.ToString()); j++;}});
        @:}
        @:catch (Platform::Exception ^exception)
        @:{
          @:NodeRT::Utils::ThrowWinRtExceptionInJs(exception);
          @:return scope.Close(Undefined());
        @:}
      @:}
      }
      else
      {
        ThrowException(Exception::Error(NodeRT::Utils::NewString(L"Invalid arguments, no suitable constructor found")));
        return scope.Close(Undefined());
      }

      args.This()->SetHiddenValue(String::NewSymbol("__winRtInstance__"), True());

      @(Model.Name) *wrapperInstance = new @(Model.Name)(winRtInstance);
      wrapperInstance->Wrap(args.This());

      return args.This();
    }
