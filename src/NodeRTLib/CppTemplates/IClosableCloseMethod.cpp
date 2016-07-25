    static Handle<Value> @(TX.CSharpMethodToCppMethod(Model.Name))(Nan::NAN_METHOD_ARGS_TYPE info)
    {
      HandleScope scope;

      if (!NodeRT::Utils::IsWinRtWrapperOf<@(TX.ToWinRT(Model.Overloads[0].DeclaringType,true))>(info.This()))
      {
        return scope.Close(Undefined());
      }

      @(Model.Overloads[0].DeclaringType.Name) *wrapper = @(Model.Overloads[0].DeclaringType.Name)::Unwrap<@(Model.Overloads[0].DeclaringType.Name)>(info.This());

      if (info.Length() == 0)
      {
        try
        {
          delete wrapper->_instance;
          wrapper->_instance = nullptr;
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
        Nan::ThrowError(Nan::Error(NodeRT::Utils::NewString(L"Bad arguments: no suitable overload found")));
        return scope.Close(Undefined());
      }

      return scope.Close(Undefined());
    }

