@{
  foreach(var overload in Model.Overloads) {
    @:static @(Model.Name)(@(TX.GetParamsFromTsMethodForDefinitions(overload, isAsync: true))): void ;
  }
}
