@{
  foreach(var overload in Model.Overloads) {
@:cls.@(Model.Name) = function @(Model.Name)(@(TX.GetParamsFromJsMethodForDefinitions(overload))) @TX.JsDefinitionTemplates.SyncMethodBody(overload)
  }
}