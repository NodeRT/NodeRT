    static Handle<Value> @(TX.CSharpMethodToCppMethod(Model.Name))(Nan::NAN_METHOD_ARGS_TYPE info)
    {
      HandleScope scope;
      @{int c = 0;}
      @foreach(var overload in Model.Overloads)
      {
        int i = 0;
        var elseString = "";
        if (c > 0) {
        elseString = "else ";
        }
        var inParamsCount = 0;
        bool methodHasOutParams = false;
        foreach (var paramInfo in overload.GetParameters())
        {
          if (!paramInfo.IsOut) 
            inParamsCount++;
          else
            methodHasOutParams = true;
        }
      @:@(elseString)if (info.Length() == @(inParamsCount)@{if (inParamsCount==0)@(")")}

        foreach (var paramInfo in overload.GetParameters())
        {
          if (paramInfo.IsOut)
            continue;
        @:&& @(String.Format(Converter.TypeCheck(paramInfo.ParameterType, TX.MainModel.Types.ContainsKey(paramInfo.ParameterType)), "info[" + i.ToString() + "]"))@{if (inParamsCount==(i+1)) @(")")}
          i++;
        }
      @:{
        @:try
        @:{
          int parameterCounter = 0;
          foreach (var paramInfo in overload.GetParameters())
          {
          var winrtConversionInfo = Converter.ToWinRT(paramInfo.ParameterType, TX.MainModel.Types.ContainsKey(paramInfo.ParameterType));   

          if (paramInfo.ParameterType.IsByRef)
          {
            winrtConversionInfo = Converter.ToWinRT(paramInfo.ParameterType.GetElementType(), TX.MainModel.Types.ContainsKey(paramInfo.ParameterType.GetElementType()));   
          }

          if (paramInfo.IsOut)
          {
            if (paramInfo.ParameterType.IsArray)
            {
          @:#error please initialize array below with ...=ref new ::Platform::Array<...>(size);
            }
          @:@(winrtConversionInfo[0]) arg@(parameterCounter);  @if (paramInfo.ParameterType.IsArray) {@("// = ref new ::Platform::Array<" + TX.ToWinRT(paramInfo.ParameterType.GetElementType()) + ">(size);")}
          }
          else
          {
          @:@(winrtConversionInfo[0]) arg@(parameterCounter) = @(string.Format(winrtConversionInfo[1], "info[" +parameterCounter + "]" ));
          }
          parameterCounter++;
          }
          
          if (overload.GetParameters().Length > 0)
          {
          @:
          }
          if (Model.Overloads[0].ReturnType == typeof(void))
          {
          @:@(TX.ToWinRT(Model.Overloads[0].DeclaringType, false))::@(TX.CSharpMethodToCppMethod(overload.Name))(@{int j=0;foreach(var paramInfo in overload.GetParameters()){if(j>0)@(", "); if (paramInfo.ParameterType.IsByRef){@("&")} @("arg" + j.ToString()); j++;}});
          }
          else
          {
          var winrtConversionInfo = Converter.ToWinRT(overload.ReturnType, TX.MainModel.Types.ContainsKey(overload.ReturnType)); 
          @:@(winrtConversionInfo[0]) result;
            var jsConversionInfo = Converter.ToJS(overload.ReturnType, TX.MainModel.Types.ContainsKey(overload.ReturnType)); 
          @:result = @(TX.ToWinRT(overload.DeclaringType, false))::@(TX.CSharpMethodToCppMethod(overload.Name))(@{int j=0;foreach(var paramInfo in overload.GetParameters()){if(j>0)@(", "); if (paramInfo.ParameterType.IsByRef){@("&")} @("arg" + j.ToString()); j++;}});
          }

          if (methodHasOutParams)
          {
          @:Handle<Object> resObj = Object::New();
          
            if (Model.Overloads[0].ReturnType != typeof(void))
            {
            var jsConversionInfo = Converter.ToJS(overload.ReturnType, TX.MainModel.Types.ContainsKey(overload.ReturnType));
          
          @:resObj->Set(String::NewSymbol("@(Converter.ToOutParameterName(overload.ReturnType))"), @(string.Format(jsConversionInfo[1], "result")));
            }
            parameterCounter = 0;
            foreach (var paramInfo in overload.GetParameters())
            {
              if (paramInfo.IsOut)
              {
                var paramJsConversionInfo = Converter.ToJS(paramInfo.ParameterType, TX.MainModel.Types.ContainsKey(paramInfo.ParameterType)); 
          @:resObj->Set(String::NewSymbol("@(paramInfo.Name)"), @(string.Format(paramJsConversionInfo[1], "arg" + parameterCounter.ToString())));
              }
              parameterCounter++;
            }

          @:return scope.Close(resObj);
          }
          else
          {
            if (Model.Overloads[0].ReturnType != typeof(void))
            {
              var jsConversionInfo = Converter.ToJS(Model.Overloads[0].ReturnType, TX.MainModel.Types.ContainsKey(Model.Overloads[0].ReturnType)); 
          @:return scope.Close(@string.Format(jsConversionInfo[1], "result"));
            }
            else
            {
          @:return scope.Close(Undefined());   
            }
          }

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
        Nan::ThrowError(Nan::Error(NodeRT::Utils::NewString(L"Bad arguments: no suitable overload found")));
        return scope.Close(Undefined());
      }

      return scope.Close(Undefined());
    }
