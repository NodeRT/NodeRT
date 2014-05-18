    static Handle<Value> @(TX.CSharpMethodToCppMethod(Model.Name))(const v8::Arguments& args)
    {
      HandleScope scope;
      ThrowException(Exception::Error(NodeRT::Utils::NewString(L"Not implemented")));
      return scope.Close(Undefined());
    }
