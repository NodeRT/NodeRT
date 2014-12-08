  static v8::Handle<v8::Value> Init@(Model.Name)Enum(const Handle<Object> exports)
  {
    HandleScope scope;
    
    Handle<Object> enumObject = Object::New();
    exports->Set(String::NewSymbol("@(Model.Name)"), enumObject);
    @foreach(var name in Enum.GetNames(Model)) {
    @:enumObject->Set(String::NewSymbol("@(TX.Uncap(name))"), Integer::New(static_cast<int>(@TX.ToWinRT(Model)::@(name))));
    }

    return scope.Close(Undefined());
  }


