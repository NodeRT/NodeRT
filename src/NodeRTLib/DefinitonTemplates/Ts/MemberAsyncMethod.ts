@{
  foreach(var overload in Model.Overloads) {
    @:@(Model.Name)(@(TX.GetParamsFromTsMethodForDefinitions(overload, isAsync: true))): void ;
  }
}