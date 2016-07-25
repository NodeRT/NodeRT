@{
  Type t = Model.EventInfo.EventHandlerType;
  Type [] eventArgs = t.GetMethods().Where((methodInfo) => { return (methodInfo.Name == "Invoke"); }).First().GetParameters().Select<System.Reflection.ParameterInfo, Type>((pi) => { return pi.ParameterType; }).ToArray();

  var foreachArg = new Func<String, int, String>((format, len) => {
    return TX.ForEachType(eventArgs, format, len);
  });
}
        try
        {
          std::shared_ptr<Persistent<Object>> callbackObjPtr(new Persistent<Object>(Persistent<Object>::New(NodeRT::Utils::CreateCallbackObjectInDomain(callback))), 
            [] (Persistent<Object> *ptr ) {
              NodeUtils::Async::RunOnMain([ptr]() {
                ptr->Dispose();
                delete ptr;
            });
          });

          @if (Model.IsStatic)
          {
          @:registrationToken = @(TX.ToWinRT(Model.EventInfo.DeclaringType, false))::@(Model.EventInfo.Name)::add(
          }
          else
          {
          @:registrationToken = wrapper->_instance->@(Model.EventInfo.Name)::add(
          }
            ref new @(TX.ToWinRT(Model.EventInfo.EventHandlerType,false))(
            [callbackObjPtr](@foreachArg("{1} arg{2}, ", 2)) {
              NodeUtils::Async::RunOnMain([callbackObjPtr @foreachArg(", arg{2}", 0)]() {
                TryCatch tryCatch;
              
                Handle<Value> error;
                @{var j = 0;}
                @foreach (var type in eventArgs)
                {
                 var jsConversionInfo = Converter.ToJS(type, TX.MainModel.Types.ContainsKey(type)); 
                @:Handle<Value> wrappedArg@(j) = @(string.Format(jsConversionInfo[1], String.Format("arg{0}",j )));
                  j++;
                }

                if (tryCatch.HasCaught())
                {
                  error = tryCatch.Exception()->ToObject();
                }
                else 
                {
                  error = Undefined();
                }

                @{var i = 0;}
                @foreach (var type in eventArgs)
                {
                @:if (wrappedArg@(i).IsEmpty()) wrappedArg@(i) = Undefined();
                  i++;
                }

                @if (eventinfo.Length > 0)
                {
                @:Handle<Value> info[] = { @foreachArg("wrappedArg{2}, ", 2) };
                @:NodeRT::Utils::CallCallbackInDomain(*callbackObjPtr, _countof(args), args);
                }
                else
                {
                @:NodeRT::Utils::CallCallbackInDomain(*callbackObjPtr, 0, nullptr);
                }
              });
            })
          );
        }
        catch (Platform::Exception ^exception)
        {
          NodeRT::Utils::ThrowWinRtExceptionInJs(exception);
          return scope.Close(Undefined());
        }
