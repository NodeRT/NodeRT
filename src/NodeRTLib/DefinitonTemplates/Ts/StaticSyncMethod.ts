@{
  foreach(var overload in Model.Overloads) {
    @:static @(Model.Name)(@(TX.GetParamsFromTsMethodForDefinitions(overload))): @(Converter.ToJsDefinitonType(overload.ReturnType, TX.MainModel.Types.ContainsKey(overload.ReturnType)));
  }
}
