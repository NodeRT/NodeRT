@{
  foreach(var overload in Model.Overloads) {
@:cls.prototype.@(Model.Name) = function @(Model.Name)(@(TX.GetParamsFromJsMethodForDefinitions(overload, isAsync: true))) @TX.JsDefinitionTemplates.AsyncMethodBody(overload)
  }
}