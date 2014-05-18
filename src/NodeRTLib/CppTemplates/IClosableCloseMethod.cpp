    static Handle<Value> @(TX.CSharpMethodToCppMethod(Model.Name))(const v8::Arguments& args)
    {
      HandleScope scope;

      if (!NodeRT::Utils::IsWinRtWrapperOf<@(TX.ToWinRT(Model.Overloads[0].DeclaringType,true))>(args.This()))
      {
        return scope.Close(Undefined());
      }

      @(Model.Overloads[0].DeclaringType.Name) *wrapper = @(Model.Overloads[0].DeclaringType.Name)::Unwrap<@(Model.Overloads[0].DeclaringType.Name)>(args.This());

      if (args.Length() == 0)
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
        ThrowException(Exception::Error(NodeRT::Utils::NewString(L"Bad arguments: no suitable overload found")));
        return scope.Close(Undefined());
      }

      return scope.Close(Undefined());
    }

