  class @(Model.Name) : public WrapperBase
  {
  public:@TX.Templates.InitExports(Model)

    virtual ::Platform::Object^ GetObjectInstance() const override
    {
      return _instance;
    }

  private:
    
    @(Model.Name)(@TX.ToWinRT(Model.Type) instance)
    {
      _instance = instance;
    }
    
    @TX.Templates.Constructor(Model)

  @foreach(var methodInfo in Model.MemberAsyncMethods) { 
      @TX.Templates.MemberASyncMethod(methodInfo)
  }
  
  @foreach(var methodInfo in Model.MemberSyncMethods) 
  { 
      if (TX.IsMethodNotImplemented(methodInfo))
      {
      @TX.Templates.NotImplementedMethod(methodInfo)
      }
      else
      {
        if (TX.IsIClosableClose(methodInfo.Overloads[0]))
        {
      @TX.Templates.IClosableCloseMethod(methodInfo)
        }
        else
        {
      @TX.Templates.MemberSyncMethod(methodInfo)
        }
      }
  }

  @foreach(var methodInfo in Model.StaticAsyncMethods) 
  { 
      @TX.Templates.StaticASyncMethod(methodInfo)
  }

  @foreach(var methodInfo in Model.StaticSyncMethods) 
  { 
      @TX.Templates.StaticSyncMethod(methodInfo)
  }

  @foreach(var propertyInfo in Model.MemberProperties) 
  {
    @TX.Templates.MemberPropertyGetter(propertyInfo)
    
    @:
    if (propertyInfo.GetSetMethod() != null) {
      @TX.Templates.MemberPropertySetter(propertyInfo)
    @:
    }
  }

  @if (Model.StaticProperties.Length > 0)
  {
    @foreach(var propertyInfo in Model.StaticProperties) 
    {
    @TX.Templates.StaticPropertyGetter(propertyInfo)
    
    @:
      if (propertyInfo.GetSetMethod() != null) {
      @TX.Templates.StaticPropertySetter(propertyInfo)
    @:
      }
    }
  }

  @if(Model.Events.Length > 0) 
  {
    @TX.Templates.Event(Model)
  }
  private:
    @(TX.ToWinRT(Model.Type)) _instance;
    static Persistent<FunctionTemplate> s_constructorTemplate;

    friend v8::Handle<v8::Value> Wrap@(Model.Name)(@(TX.ToWinRT(Model.Type)) wintRtInstance);
    friend @(TX.ToWinRT(Model.Type)) Unwrap@(Model.Name)(Handle<Value> value);
    friend bool Is@(Model.Name)Wrapper(Handle<Value> value);
  };
  Persistent<FunctionTemplate> @(Model.Name)::s_constructorTemplate;

  v8::Handle<v8::Value> Wrap@(Model.Name)(@(TX.ToWinRT(Model.Type)) winRtInstance)
  {
    HandleScope scope;

    if (winRtInstance == nullptr)
    {
      return scope.Close(Undefined());
    }

    Handle<Object> opaqueWrapper = CreateOpaqueWrapper(winRtInstance);
    Handle<Value> args[] = {opaqueWrapper};
    return scope.Close(@(Model.Name)::s_constructorTemplate->GetFunction()->NewInstance(_countof(args), args));
  }

  @(TX.ToWinRT(Model.Type)) Unwrap@(Model.Name)(Handle<Value> value)
  {
     return @(Model.Name)::Unwrap<@(Model.Name)>(value.As<Object>())->_instance;
  }

  void Init@(Model.Name)(Handle<Object> exports)
  {
    @(Model.Name)::Init(exports);
  }

