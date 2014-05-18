  
  static bool Is@(Model.Name)JsObject(Handle<Value> value)
  {
    if (!value->IsObject())
    {
      return false;
    }

    Handle<String> symbol;
    Handle<Object> obj = value.As<Object>();

    @foreach (var field in Model.GetFields())
    {
    @:symbol = String::NewSymbol("@TX.Uncap(field.Name)");
    @:if (obj->Has(symbol))
    @:{
    @:  if (!@(String.Format(Converter.TypeCheck(field.FieldType, TX.MainModel.Types.ContainsKey(field.FieldType)), "obj->Get(symbol)")))
    @:  {
    @:      return false;
    @:  }
    @:}
    @:
    }
    return true;
  }

  @TX.ToWinRT(Model) @(Model.Name)FromJsObject(Handle<Value> value)
  {
    HandleScope scope;
    @TX.ToWinRT(Model) returnValue;
    
    if (!value->IsObject())
    {
      ThrowException(Exception::TypeError(NodeRT::Utils::NewString(L"Unexpected type, expected an object")));
      return returnValue;
    }

    Handle<Object> obj = value.As<Object>();
    Handle<String> symbol;

    @foreach (var field in Model.GetFields())
    {
    @:symbol = String::NewSymbol("@TX.Uncap(field.Name)");
    @:if (obj->Has(symbol))
    @:{
      var winRtConversionInfo = Converter.ToWinRT(field.FieldType);
    @:  returnValue.@(field.Name) = @(String.Format(winRtConversionInfo[1],"obj->Get(symbol)"));
    @:}
    @:
    }
    return returnValue;
  }

  Handle<Value> @(Model.Name)ToJsObject(@TX.ToWinRT(Model) value)
  {
    HandleScope scope;

    Handle<Object> obj = Object::New();

    @foreach (var field in Model.GetFields())
    {
    var jsConversionInfo = Converter.ToJS(field.FieldType, TX.MainModel.Types.ContainsKey(field.FieldType)); 
    @:obj->Set(String::NewSymbol("@TX.Uncap(field.Name)"), @string.Format(jsConversionInfo[1], "value." + field.Name));
    }
    
    return scope.Close(obj);
  }

