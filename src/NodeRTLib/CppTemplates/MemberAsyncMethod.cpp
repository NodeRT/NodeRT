    static Handle<Value> @(Model.Name)(const v8::Arguments& args)
    {
      HandleScope scope;

      if (!NodeRT::Utils::IsWinRtWrapperOf<@(TX.ToWinRT(Model.Overloads[0].DeclaringType,true))>(args.This()))
      {
        return scope.Close(Undefined());
      }

      if (args.Length() == 0 || !args[args.Length() -1]->IsFunction())
      {
          ThrowException(Exception::Error(NodeRT::Utils::NewString(L"Bad arguments: No callback was given")));
          return scope.Close(Undefined());
      }

      @(Model.Overloads[0].DeclaringType.Name) *wrapper = @(Model.Overloads[0].DeclaringType.Name)::Unwrap<@(Model.Overloads[0].DeclaringType.Name)>(args.This());

      @TX.ToWinRT(Model.Overloads[0].ReturnType) op;
    
      @{int c = 0;}
      @foreach(var overload in Model.Overloads)
      {
        int i = 0;
        var elseString = "";
        if (c > 0) {
        elseString = "else ";
        }
      @:@(elseString)if (args.Length() == @(overload.GetParameters().Length+1)@{if (overload.GetParameters().Length==0)@(")")}

        foreach (var paramInfo in overload.GetParameters())
        {
        
        @:&& @(String.Format(Converter.TypeCheck(paramInfo.ParameterType, TX.MainModel.Types.ContainsKey(paramInfo.ParameterType)), "args[" + i.ToString() + "]"))@{if (overload.GetParameters().Length==(i+1)) @(")")}
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
          @:op = wrapper->_instance->@(overload.Name)(@{int j=0;foreach(var paramInfo in overload.GetParameters()){if(j>0)@(","); @("arg" + j.ToString()); j++;}});
        @:}
        @:catch (Platform::Exception ^exception)
        @:{
          @:NodeRT::Utils::ThrowWinRtExceptionInJs(exception);
          @:return scope.Close(Undefined());
        @:}
      @:}
        c++;
      }
      else 
      {
        ThrowException(Exception::Error(NodeRT::Utils::NewString(L"Bad arguments: no suitable overload found")));
        return scope.Close(Undefined());
      }
    
      auto opTask = create_task(op);
      uv_async_t* asyncToken = NodeUtils::Async::GetAsyncToken(args[args.Length() -1].As<Function>());
      @{
        System.Reflection.MethodInfo[] returnTypeMethods = Model.Overloads[0].ReturnType.GetMethods();
        Type taskReturnType = returnTypeMethods.Where((methodInfo) => { return (methodInfo.Name == "GetResults"); }).First().ReturnType;
        string taskGenericType = TX.ToWinRT(taskReturnType);
      }
      opTask.then( [asyncToken] (task<@(taskGenericType)> t) 
      {	
        try
        {
          @{
            if (taskReturnType != typeof(void))
            {
          @:auto result = t.get();
          @:NodeUtils::Async::RunCallbackOnMain(asyncToken, [result](NodeUtils::InvokeCallbackDelegate invokeCallback) {
            }
            else
            {
          @:t.get();
          @:NodeUtils::Async::RunCallbackOnMain(asyncToken, [](NodeUtils::InvokeCallbackDelegate invokeCallback) {
            }
          }

            @{
              if (taskReturnType == typeof(void))
              {
            @:Handle<Value> args[] = {Undefined()};
              }
              else
              {
                  var jsConversionInfo = Converter.ToJS(taskReturnType, TX.MainModel.Types.ContainsKey(taskReturnType)); 
            @:TryCatch tryCatch;
            @:Handle<Value> error; 
            @:Handle<Value> arg1 = @string.Format(jsConversionInfo[1], "result");

            @:if (tryCatch.HasCaught())
            @:{
            @:  error = tryCatch.Exception()->ToObject();
            @:}
            @:else 
            @:{
            @:  error = Undefined();
            @:}

            @:if (arg1.IsEmpty()) arg1 = Undefined();

            @:Handle<Value> args[] = {error, arg1};
              }
            }
            invokeCallback(_countof(args), args);
          });
        }
        catch (Platform::Exception^ exception)
        {
          NodeUtils::Async::RunCallbackOnMain(asyncToken, [exception](NodeUtils::InvokeCallbackDelegate invokeCallback) {
             
            Handle<Value> error = NodeRT::Utils::WinRtExceptionToJsError(exception);
        
            Handle<Value> args[] = {error};
            invokeCallback(_countof(args), args);
          });
        }  		
      });

      return scope.Close(Undefined());
    }
