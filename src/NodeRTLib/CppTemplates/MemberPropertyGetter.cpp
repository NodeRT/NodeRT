    static Handle<Value> @(Model.Name)Getter(Local<String> property, const AccessorInfo &info)
    {
      HandleScope scope;
      
      if (!NodeRT::Utils::IsWinRtWrapperOf<@(TX.ToWinRT(Model.DeclaringType,true))>(info.This()))
      {
        return scope.Close(Undefined());
      }

      @(Model.DeclaringType.Name) *wrapper = @(Model.DeclaringType.Name)::Unwrap<@(Model.DeclaringType.Name)>(info.This());

      try 
      {@{
          var jsConversionInfo = Converter.ToJS(Model.PropertyType, TX.MainModel.Types.ContainsKey(Model.PropertyType)); 
          var winrtConversionInfo = Converter.ToWinRT(Model.PropertyType);}
        @winrtConversionInfo[0] result = wrapper->_instance->@(Model.Name);
        return scope.Close(@string.Format(jsConversionInfo[1], "result"));
      }
      catch (Platform::Exception ^exception)
      {
        NodeRT::Utils::ThrowWinRtExceptionInJs(exception);
        return scope.Close(Undefined());
      }
    }
