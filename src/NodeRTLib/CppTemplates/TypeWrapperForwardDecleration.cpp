@foreach(var t in Model.Types.Values) {
  @:v8::Handle<v8::Value> Wrap@(t.Name)(@(TX.ToWinRT(t.Type)) wintRtInstance);
  @:@TX.ToWinRT(t.Type) Unwrap@(t.Name)(Handle<Value> value);
  @:
}