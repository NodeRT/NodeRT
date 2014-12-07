  static v8::Handle<v8::Value> Init@(Model.Name)Enum(const Handle<Object> exports)
  {
    HandleScope scope;
    
    Handle<Object> enumObject = Object::New();
    exports->Set(String::NewSymbol("@(Model.Name)"), enumObject);
    @{var counter = 0;}
    @foreach(var field in Model.DeclaredFields) {
      if (counter != 0)
      {
    @:enumObject->Set(String::NewSymbol("@(TX.Uncap(field.Name))"), Integer::New(static_cast<int>(@TX.ToWinRT(Model)::@(field.Name))));
      }
      counter++;
    }

    return scope.Close(Undefined());
  }


