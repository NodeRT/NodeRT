    
    static v8::Handle<v8::Value> Init(const Handle<Object> exports)
    {
      HandleScope scope;
      
      s_constructorTemplate = Persistent<FunctionTemplate>::New(FunctionTemplate::New(New));
      s_constructorTemplate->SetClassName(String::NewSymbol("@(Model.Name)"));
      s_constructorTemplate->InstanceTemplate()->SetInternalFieldCount(1);
      
      @if(Model.MemberASyncMethods.Length > 0 || Model.StaticASyncMethods.Length > 0)
      {
      @:Handle<Value> asyncSymbol = String::NewSymbol("__winRtAsync__");
      @:Handle<Function> func;
      }
      @if(Model.MemberSyncMethods.Length > 0) {
      @:
      foreach(var method in Model.MemberSyncMethods) {
      @:s_constructorTemplate->PrototypeTemplate()->Set(String::NewSymbol("@(TX.Uncap(TX.CSharpMethodToCppMethod(method.Name)))"), FunctionTemplate::New(@(TX.CSharpMethodToCppMethod(method.Name)))->GetFunction());
        }
      @:
      }
      @if(Model.MemberASyncMethods.Length > 0) {
      @:
      foreach(var method in Model.MemberASyncMethods) {
      @:func = FunctionTemplate::New(@TX.CSharpMethodToCppMethod(method.Name))->GetFunction();
      @:func->Set(asyncSymbol, True(), PropertyAttribute::DontEnum);
      @:s_constructorTemplate->PrototypeTemplate()->Set(String::NewSymbol("@(TX.Uncap(TX.CSharpMethodToCppMethod(method.Name)))"), func);
        }
      @:
      }
      @if(Model.HasMemberEvents) {  
      @:
      @:Local<Function> addListenerFunc = FunctionTemplate::New(AddListener)->GetFunction();
      @:s_constructorTemplate->PrototypeTemplate()->Set(String::NewSymbol("addListener"), addListenerFunc);
      @:s_constructorTemplate->PrototypeTemplate()->Set(String::NewSymbol("on"), addListenerFunc);
            
      @:Local<Function> removeListenerFunc = FunctionTemplate::New(RemoveListener)->GetFunction();
      @:s_constructorTemplate->PrototypeTemplate()->Set(String::NewSymbol("removeListener"), removeListenerFunc);
      @:s_constructorTemplate->PrototypeTemplate()->Set(String::NewSymbol("off"), removeListenerFunc);
      }
      @if(Model.MemberProperties.Length > 0) {
      @:
      foreach(var prop in Model.MemberProperties) {
        var propName = TX.Uncap(prop.Name);
      if (prop.GetSetMethod() != null) {
      @:s_constructorTemplate->PrototypeTemplate()->SetAccessor(String::NewSymbol("@(propName)"), @(prop.Name)Getter, @(prop.Name)Setter);
      }
      else {
      @:s_constructorTemplate->PrototypeTemplate()->SetAccessor(String::NewSymbol("@(propName)"), @(prop.Name)Getter);
      }
      }}
      
      Local<Function> constructor = s_constructorTemplate->GetFunction();

      @{
        if(Model.StaticSyncMethods.Length > 0) {
        foreach(var method in Model.StaticSyncMethods) {
      @:constructor->Set(String::NewSymbol("@(TX.Uncap(TX.CSharpMethodToCppMethod(method.Name)))"), FunctionTemplate::New(@TX.CSharpMethodToCppMethod(method.Name))->GetFunction());
          }
        }
        if(Model.StaticASyncMethods.Length > 0) {
          foreach(var method in Model.StaticASyncMethods) {
      @:func = FunctionTemplate::New(@TX.CSharpMethodToCppMethod(method.Name))->GetFunction();
      @:func->Set(asyncSymbol, True(), PropertyAttribute::DontEnum);
      @:constructor->Set(String::NewSymbol("@(TX.Uncap(TX.CSharpMethodToCppMethod(method.Name)))"), func);
          }
        }

        if(Model.StaticProperties.Length > 0) {
          foreach(var prop in Model.StaticProperties) {
            var propName = TX.Uncap(prop.Name);
            if (prop.GetSetMethod() != null) 
            {
      @:constructor->SetAccessor(String::NewSymbol("@(propName)"), @(prop.Name)Getter, @(prop.Name)Setter);
            }
            else 
            {
      @:constructor->SetAccessor(String::NewSymbol("@(propName)"), @(prop.Name)Getter);
            }
          }
        }

        if (Model.HasStaticEvents)
        {
      @:
          if (!Model.HasMemberEvents)
          {
      @:Local<Function> addListenerFunc = FunctionTemplate::New(AddListener)->GetFunction();
          }
      @:constructor->Set(String::NewSymbol("addListener"), addListenerFunc);
      @:constructor->Set(String::NewSymbol("on"), addListenerFunc);
            
          if (!Model.HasMemberEvents)
          {
      @:Local<Function> removeListenerFunc = FunctionTemplate::New(RemoveListener)->GetFunction();
          }
          
      @:constructor->Set(String::NewSymbol("removeListener"), removeListenerFunc);
      @:constructor->Set(String::NewSymbol("off"), removeListenerFunc);
        }
      }
      exports->Set(String::NewSymbol("@(Model.Name)"), constructor);
      return scope.Close(Undefined());
    }
