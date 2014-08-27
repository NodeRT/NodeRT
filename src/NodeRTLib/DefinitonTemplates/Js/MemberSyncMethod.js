@{
  foreach(var overload in Model.Overloads) {
@:cls.prototype.@(Model.Name) = function @(Model.Name)(@(TX.GetParamsFromJsMethodForDefinitions(overload))) @TX.JsDefinitionTemplates.SyncMethodBody(overload)
  }
}