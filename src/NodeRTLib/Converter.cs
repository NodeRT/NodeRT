// Copyright (c) Microsoft Corporation
// All rights reserved. 
//
// Licensed under the Apache License, Version 2.0 (the ""License""); you may not use this file except in compliance with the License. You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0 
//
// THIS CODE IS PROVIDED ON AN  *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED WARRANTIES OR CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE, MERCHANTABLITY OR NON-INFRINGEMENT. 
//
// See the Apache Version 2.0 License for specific language governing permissions and limitations under the License.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace NodeRTLib
{
    public static class Converter
    {
        /// <summary>
        /// Convert a C# dataType to its equivlant JavaScript dataType, here used to generate TypeScript and JavaScript definition files
        /// </summary>
        /// <param name="type">C# dataType</param>
        /// <param name="typeIsInNameSpace">A flag indicating if teh Type is in the nameSpace</param>
        /// <returns>Equivlant JS dataType</returns>
        public static string ToJsDefinitonType(Type type, bool typeIsInNameSpace = false)
        {
            if (type.IsPrimitive)
            {
                if (type == typeof(Byte) ||
                    type == typeof(SByte) ||
                    type == typeof(Int16) ||
                    type == typeof(UInt16) ||
                    type == typeof(Int32) ||
                    type == typeof(UInt32)
                    )
                {
                    return "Number";
                }
            }

            if (type.IsByRef)
            {
                return ToJsDefinitonType(type.GetElementType(), typeIsInNameSpace);
            }

            if (type.IsGenericType && type.GetGenericTypeDefinition() == typeof(Nullable<>))
            {
                return ToJsDefinitonType(type.GetGenericArguments()[0]);
            }

            if (type.IsArray)
            {
                return string.Format("Array<{0}>", ToJsDefinitonType(type.GetElementType(), typeIsInNameSpace));
            }

            if (type.IsEnum)
            {
                if (type.Namespace.Equals(TX.MainModel.winrtnamespace))
                {
                    return type.Name;
                }
                return "Number";
            }

            if (type == typeof(void))
            {
                return "void";
            }

            if (type.IsGenericType && type.FullName != null && (type.FullName.StartsWith("Windows.Foundation.Collections") ||
               type.FullName.StartsWith("System.Collections")))
            {
                return "Object";
            }

            string[] jsType = ToJS(type, typeIsInNameSpace);

            if (jsType[0].Equals("Value"))
            {
                if (type.IsValueType)
                {
                    return type.Name;
                }
                return "Object";
            }

            return jsType[0];
        }

        public static string[] ToJS(Type type, bool typeIsInNameSpace = false)
        {
            // The primitive types are Boolean, Byte, SByte, Int16, UInt16, Int32, UInt32, Int64, UInt64, IntPtr, UIntPtr, Char, Double, and Single.
            if (type.IsPrimitive)
            {
                if (type == typeof(Byte) ||
                    type == typeof(SByte) ||
                    type == typeof(Int16) ||
                    type == typeof(UInt16) ||
                    type == typeof(Int32) ||
                    type == typeof(UInt32)
                    )
                {
                    return new[] { "Integer", "Integer::New({0})" };
                }
                else if (type == typeof(UInt32))
                {
                    return new[] { "Integer", "Integer::NewFromUnsigned({0})" };
                }
                else if (type == typeof(Int64) ||
                    type == typeof(UInt64) ||
                    type == typeof(Double) ||
                    type == typeof(Single) ||
                    type == typeof(IntPtr) ||
                    type == typeof(UIntPtr))
                {
                    return new[] { "Number", "Number::New(static_cast<double>({0}))" };
                }
                else if (type == typeof(Boolean))
                {
                    return new[] { "Boolean", "Boolean::New({0})" };
                }
                else if (type == typeof(Char))
                {
                    return new[] { "String", "NodeRT::Utils::JsStringFromChar({0})" };
                }
            }

            if (type.IsByRef)
            {
                return ToJS(type.GetElementType(), typeIsInNameSpace);
            }

            if (type == typeof(String))
            {
                return new[] { "String", "NodeRT::Utils::NewString({0}->Data())" };
            }

            if (type == typeof(DateTimeOffset) || type == typeof(DateTime))
            {
                // 116444736000000000 = The time 100 nanoseconds between 1/1/1970(UTC) to 1/1/1601(UTC)
                // ux_time = (Current time since 1601 in 100 nano sec units)/10000 - 11644473600000;
                return new[] { "Date", "Date::New({0}.UniversalTime/10000.0 - 11644473600000)" };
            }

            if (type == typeof(Exception))
            {
                return new[] { "Number", "Integer::New({0}.Value)" };
            }

            if (type == typeof(TimeSpan))
            {
                // convert 100 nano seconds units to millisecond
                // the conversion factor is 10000
                return new[] { "Number", "Number::New({0}.Duration/10000.0)" };
            }

            if (type.IsGenericType && type.GetGenericTypeDefinition() == typeof(Nullable<>))
            {
                String[] conversionInfo = ToJS(type.GetGenericArguments()[0]);

                conversionInfo[1] = "{0} ? " + String.Format(conversionInfo[1], "{0}->Value") + ": static_cast<Handle<Value>>(Undefined())";
                return conversionInfo;
            }

            if (type.FullName == "Windows.UI.Color")
            {
                return new[] { "Object", "NodeRT::Utils::ColorToJs({0})" };
            }

            if (type.FullName == "Windows.Foundation.Point")
            {
                return new[] { "Object", "NodeRT::Utils::PointToJs({0})" };
            }

            if (type.FullName == "Windows.Foundation.Rect")
            {
                return new[] { "Object", "NodeRT::Utils::RectToJs({0})" };
            }

            if (type.FullName == "Windows.Foundation.Size")
            {
                return new[] { "Object", "NodeRT::Utils::SizeToJs({0})" };
            }

            if (type.IsEnum)
            {
                return new[] { "Integer", "Integer::New(static_cast<int>({0}))" };
            }

            if (type.IsArray)
            {
                return ToCollection("Array", type.GetElementType());
            }

            if (type == typeof(Guid))
            {
                return new[] { "String", "NodeRT::Utils::GuidToJs({0})" };
            }

            if (type.IsGenericType && type.FullName != null && (type.FullName.StartsWith("Windows.Foundation.Collections.IIterator`1") ||
                type.FullName.StartsWith("System.Collections.Generic.IEnumerator`1")))
            {
                return ToReadOnlyCollection("Iterator", type.GetGenericArguments()[0]);
            }

            if (type.IsGenericType && type.FullName != null &&  (type.FullName.StartsWith("Windows.Foundation.Collections.IITerable`1") || 
                type.FullName.StartsWith("System.Collections.Generic.IEnumerable`1")))
            {
                return ToReadOnlyCollection("Iterable", type.GetGenericArguments()[0]);
            }

            if (type.IsGenericType && type.FullName != null &&  type.FullName.StartsWith("System.Collections.Generic.IReadOnlyList`1"))
            {
                return ToCollection("VectorView", type.GetGenericArguments()[0]);
            }

            if (type.IsGenericType && type.FullName != null &&  type.FullName.StartsWith("System.Collections.Generic.IList`1"))
            {
                return ToCollection("Vector", type.GetGenericArguments()[0]);
            }

            if (type.IsGenericType && type.FullName != null && type.FullName.StartsWith("System.Collections.Generic.IReadOnlyDictionary`2"))
            {
                return ToReadOnlyKeyValueCollection("MapView", type.GetGenericArguments()[0], type.GetGenericArguments()[1]);
            }

            if (type.IsGenericType && type.FullName != null &&  type.FullName.StartsWith("System.Collections.Generic.IDictionary`2"))
            {
                return ToKeyValueCollection("Map", type.GetGenericArguments()[0], type.GetGenericArguments()[1]);
            }

            // In CS KeyValuePair is of value type so we need to make sure the check for value type is called after this
            if (type.IsGenericType && type.FullName != null && type.FullName.StartsWith("System.Collections.Generic.KeyValuePair`2"))
            {
                return ToReadOnlyKeyValue("KeyValuePair", type.GetGenericArguments()[0], type.GetGenericArguments()[1]);
            }

            if (type.IsValueType)
            {
                return new[] { "Value", type.Name + "ToJsObject({0})" };
            }

            if (typeIsInNameSpace)
            {
                return new[] { type.Name, "Wrap" + type.Name + "({0})" };
            }

            // optimization
            if (type == typeof(Object))
            {
                return new[] {"Value", "CreateOpaqueWrapper({0})"};
            }

            return new[] { "Value", "NodeRT::Utils::CreateExternalWinRTObject(" +
                "\"" + type.Namespace + "\", "+
                "\"" + type.Name + "\", "+
                "{0})" };
        }

        public static string[] ToCollection(string collectionName, Type elementType)
        {
            string elementRtType = TX.ToWinRT(elementType);
            string[] elementTypeToJs = ToJS(elementType, TX.MainModel.Types.ContainsKey(elementType));
            string checkType = TypeCheck(elementType, TX.MainModel.Types.ContainsKey(elementType));
            string[] jsToElementType = ToWinRT(elementType, TX.MainModel.Types.ContainsKey(elementType));
            // note that double curl braces here are used because String.Format will 
            string creatorFunction = "NodeRT::Collections::"+ collectionName + "Wrapper<" + elementRtType + ">::Create" + collectionName + "Wrapper({0}, \r\n" +
"            [](" + elementRtType + " val) -> Handle<Value> {{\r\n" +
"              return " + ReplaceBracketsWithDoubleBrackets(String.Format(elementTypeToJs[1], "val")) + ";\r\n" +
"            }},\r\n" +
"            [](Handle<Value> value) -> bool {{\r\n" +
"              return " + ReplaceBracketsWithDoubleBrackets(String.Format(checkType, "value")) + ";\r\n" +
"            }},\r\n" +
"            [](Handle<Value> value) -> " + elementRtType + " {{\r\n" +
"              return " + ReplaceBracketsWithDoubleBrackets(String.Format(jsToElementType[1], "value")) + ";\r\n" +
"            }}\r\n" +
"          )";

            return new[] { "NodeRT::Collections::Create" + collectionName + "<" + elementRtType + ">", creatorFunction };
        }

        public static string[] ToReadOnlyCollection(string collectionName, Type elementType)
        {
            string elementRtType = TX.ToWinRT(elementType);
            string[] elementTypeToJs = ToJS(elementType, TX.MainModel.Types.ContainsKey(elementType));

            // note that double curl braces here are used because String.Format will 
            string creatorFunction = "NodeRT::Collections::" + collectionName + "Wrapper<" + elementRtType + ">::Create" + collectionName + "Wrapper({0}, \r\n" +
"            [](" + elementRtType + " val) -> Handle<Value> {{\r\n" +
"              return " + ReplaceBracketsWithDoubleBrackets(String.Format(elementTypeToJs[1], "val")) + ";\r\n" +
"            }}\r\n" +
"          )";

            return new[] { "NodeRT::Collections::Create" + collectionName + "<" + elementRtType + ">", creatorFunction };
        }

        public static string[] ToReadOnlyKeyValue(string collectionName, Type keyType, Type valueType)
        {
            string keyRtType = TX.ToWinRT(keyType);
            string[] keyTypeToJs = ToJS(keyType, TX.MainModel.Types.ContainsKey(keyType));

            string valueRtType = TX.ToWinRT(valueType);
            string[] valueTypeToJs = ToJS(valueType, TX.MainModel.Types.ContainsKey(valueType));

            // note that double curl braces here are used because String.Format will 
            string creatorFunction = "NodeRT::Collections::" + collectionName + "Wrapper<" + keyRtType + "," + valueRtType + ">::Create" + collectionName + "Wrapper({0}, \r\n" +
"            [](" + keyRtType + " val) -> Handle<Value> {{\r\n" +
"              return " + ReplaceBracketsWithDoubleBrackets(String.Format(keyTypeToJs[1], "val")) + ";\r\n" +
"            }},\r\n" +
"            [](" + valueRtType + " val) -> Handle<Value> {{\r\n" +
"              return " + ReplaceBracketsWithDoubleBrackets(String.Format(valueTypeToJs[1], "val")) + ";\r\n" +
"            }}\r\n" +
"          )";

            return new[] { "NodeRT::Collections::Create" + collectionName + "<" + keyRtType + "," + valueRtType + ">", creatorFunction };
        }

        public static string[] ToReadOnlyKeyValueCollection(string collectionName, Type keyType, Type valueType)
        {
            string keyRtType = TX.ToWinRT(keyType);
            string[] keyTypeToJs = ToJS(keyType, TX.MainModel.Types.ContainsKey(keyType));
            string keyCheckType = TypeCheck(keyType, TX.MainModel.Types.ContainsKey(keyType));
            string[] jsToKeyElementType = ToWinRT(keyType, TX.MainModel.Types.ContainsKey(keyType));

            string valueRtType = TX.ToWinRT(valueType);
            string[] valueTypeToJs = ToJS(valueType, TX.MainModel.Types.ContainsKey(valueType));

            // note that double curl braces here are used because String.Format will 
            string creatorFunction = "NodeRT::Collections::" + collectionName + "Wrapper<" + keyRtType + "," + valueRtType + ">::Create" + collectionName + "Wrapper({0}, \r\n" +
"            [](" + keyRtType + " val) -> Handle<Value> {{\r\n" +
"              return " + ReplaceBracketsWithDoubleBrackets(String.Format(keyTypeToJs[1], "val")) + ";\r\n" +
"            }},\r\n" +
"            [](Handle<Value> value) -> bool {{\r\n" +
"              return " + ReplaceBracketsWithDoubleBrackets(String.Format(keyCheckType, "value")) + ";\r\n" +
"            }},\r\n" +
"            [](Handle<Value> value) -> " + keyRtType + " {{\r\n" +
"              return " + ReplaceBracketsWithDoubleBrackets(String.Format(jsToKeyElementType[1], "value")) + ";\r\n" +
"            }},\r\n" +
"            [](" + valueRtType + " val) -> Handle<Value> {{\r\n" +
"              return " + ReplaceBracketsWithDoubleBrackets(String.Format(valueTypeToJs[1], "val")) + ";\r\n" +
"            }}\r\n" +
"          )";

            return new[] { "NodeRT::Collections::Create" + collectionName + "<" + keyRtType + "," + valueRtType + ">", creatorFunction };
        }

        public static string[] ToKeyValueCollection(string collectionName, Type keyType, Type valueType)
        {
            string keyRtType = TX.ToWinRT(keyType);
            string[] keyTypeToJs = ToJS(keyType, TX.MainModel.Types.ContainsKey(keyType));
            string keyCheckType = TypeCheck(keyType, TX.MainModel.Types.ContainsKey(keyType));
            string[] jsToKeyElementType = ToWinRT(keyType, TX.MainModel.Types.ContainsKey(keyType));

            string valueRtType = TX.ToWinRT(valueType);
            string[] valueTypeToJs = ToJS(valueType, TX.MainModel.Types.ContainsKey(valueType));
            string valueCheckType = TypeCheck(valueType, TX.MainModel.Types.ContainsKey(valueType));
            string[] jsToValueElementType = ToWinRT(valueType, TX.MainModel.Types.ContainsKey(valueType));

            // note that double curl braces here are used because String.Format will 
            string creatorFunction = "NodeRT::Collections::" + collectionName + "Wrapper<" + keyRtType + "," + valueRtType + ">::Create" + collectionName + "Wrapper({0}, \r\n" +
"            [](" + keyRtType + " val) -> Handle<Value> {{\r\n" +
"              return " + ReplaceBracketsWithDoubleBrackets(String.Format(keyTypeToJs[1], "val")) + ";\r\n" +
"            }},\r\n" +
"            [](Handle<Value> value) -> bool {{\r\n" +
"              return " + ReplaceBracketsWithDoubleBrackets(String.Format(keyCheckType, "value")) + ";\r\n" +
"            }},\r\n" +
"            [](Handle<Value> value) -> " + keyRtType + " {{\r\n" +
"              return " + ReplaceBracketsWithDoubleBrackets(String.Format(jsToKeyElementType[1], "value")) + ";\r\n" +
"            }},\r\n" +
"            [](" + valueRtType + " val) -> Handle<Value> {{\r\n" +
"              return " + ReplaceBracketsWithDoubleBrackets(String.Format(valueTypeToJs[1], "val")) + ";\r\n" +
"            }},\r\n" +
"            [](Handle<Value> value) -> bool {{\r\n" +
"              return " + ReplaceBracketsWithDoubleBrackets(String.Format(valueCheckType, "value")) + ";\r\n" +
"            }},\r\n" +
"            [](Handle<Value> value) -> " + valueRtType + " {{\r\n" +
"              return " + ReplaceBracketsWithDoubleBrackets(String.Format(jsToValueElementType[1], "value")) + ";\r\n" +
"            }}\r\n" +
"          )";

            return new[] { "NodeRT::Collections::Create" + collectionName + "<" + keyRtType + "," + valueRtType + ">", creatorFunction };
        }

        public static string ReplaceBracketsWithDoubleBrackets(string str)
        {
            return str.Replace("{", "{{").Replace("}", "}}");
        }

        public static string[] ToWinRT(Type type, bool typeIsInNameSpace = false)
        {
            // The primitive types are Boolean, Byte, SByte, Int16, UInt16, Int32, UInt32, Int64, UInt64, IntPtr, UIntPtr, Char, Double, and Single.
            if (type.IsPrimitive)
            {
                if (type == typeof(Byte))
                {
                    return new[] { "unsigned char", "static_cast<unsigned char>({0}->Int32Value())" };
                }
                else if (type == typeof(SByte))
                {
                    return new[] { "char", "static_cast<char>({0}->Int32Value())" };
                }
                else if (type == typeof(Int16))
                {
                    return new[] { "short", "static_cast<short>({0}->Int32Value())" };
                }
                else if (type == typeof(UInt16))
                {
                    return new[] { "unsigned short", "static_cast<unsigned short>({0}->Int32Value())" };
                }
                else if (type == typeof(Int32))
                {
                    return new[] { "int", "static_cast<int>({0}->Int32Value())" };
                }
                else if (type == typeof(UInt32))
                {
                    return new[] { "unsigned int", "static_cast<unsigned int>({0}->IntegerValue())" };
                }
                else if (type == typeof(Int64))
                {
                    return new[] { "__int64", "{0}->IntegerValue()" };
                }
                else if (type == typeof(UInt64))
                {
                    return new[] { "unsigned __int64", "static_cast<unsigned __int64>({0}->IntegerValue())" };
                }
                else if (type == typeof(IntPtr))
                {
                    return new[] { "__int64", "{0}->IntegerValue()" };
                }
                else if (type == typeof(UIntPtr))
                {
                    return new[] { "unsigned __int64", "static_cast<unsigned __int64>({0}->IntegerValue())" };
                }
                else if (type == typeof(Single))
                {
                    return new[] { "float", "static_cast<float>({0}->NumberValue())" };
                }
                else if (type == typeof(Double))
                {
                    return new[] { "double", "{0}->NumberValue()" };
                }
                else if (type == typeof(Boolean))
                {
                    return new[] { "bool", "{0}->BooleanValue()" };
                }
                else if (type == typeof(Char))
                {
                    return new[] { "wchar_t", "NodeRT::Utils::GetFirstChar({0})" };
                }
            }

            if (type == typeof(String))
            {
                return new[] { "Platform::String^", "ref new Platform::String(NodeRT::Utils::StringToWchar(v8::String::Value({0})))" };
            }

            if (type == typeof(DateTime) || type == typeof(DateTimeOffset))
            {
                return new[] { "::Windows::Foundation::DateTime", "NodeRT::Utils::DateTimeFromJSDate({0})" };
            }

            if (type == typeof(Guid))
            {
                return new[] { "::Platform::Guid", "NodeRT::Utils::GuidFromJs({0})" };
            }

            if (type == typeof(Exception))
            {
                return new[] { "::Windows::Foundation::HResult", "NodeRT::Utils::HResultFromJsInteger({0}->IntegerValue())" };
            }

            if (type == typeof(TimeSpan))
            {
                return new[] { "::Windows::Foundation::TimeSpan", "NodeRT::Utils::TimeSpanFromMilli(static_cast<int64_t>({0}->NumberValue()))" };
            }

            if (type.FullName == "Windows.UI.Color")
            {
                return new[] { "::Windows::UI::Color", "NodeRT::Utils::ColorFromJs({0})" };
            }

            if (type.FullName == "Windows.Foundation.Point")
            {
                return new[] { "::Windows::Foundation::Point", "NodeRT::Utils::PointFromJs({0})" };
            }

            if (type.FullName == "Windows.Foundation.Rect")
            {
                return new[] { "::Windows::Foundation::Rect", "NodeRT::Utils::RectFromJs({0})" };
            }

            if (type.FullName == "Windows.Foundation.Size")
            {
                return new[] { "::Windows::Foundation::Size", "NodeRT::Utils::SizeFromJs({0})" };
            }

            if (type.IsGenericType && type.GetGenericTypeDefinition() == typeof(Nullable<>))
            {
                var genericArgConversionInfo = ToWinRT(type.GetGenericArguments()[0]);
                return new[] { "::Platform::IBox<" + genericArgConversionInfo[0] + ">^", "ref new ::Platform::Box<" + genericArgConversionInfo[0] + ">(" + genericArgConversionInfo[1] + ")" };
            }

            var winrtFullType = TX.ToWinRT(type);

            if (type.IsEnum)
            {
                return new[] { winrtFullType, "static_cast<" + winrtFullType + ">({0}->Int32Value())" };
            }

            // this if clause should come after IsEnum, since an enum will also return true for IsValueType
            if (type.IsValueType && !(type.IsGenericType && type.FullName.StartsWith("System.Collections.Generic.KeyValuePair`2")))
            {
                return new[] { winrtFullType, type.Name + "FromJsObject({0})" };
            }

            if (typeIsInNameSpace)
            {
                return new[] { winrtFullType, "Unwrap" + type.Name + "({0})" };
            }
            else
            {
                return new[] { winrtFullType, "dynamic_cast<" + winrtFullType + ">(NodeRT::Utils::GetObjectInstance({0}))" };
            }
        }

        public static string TypeCheck(Type type, bool typeIsInNameSpace = false)
        {
            // The primitive types are Boolean, Byte, SByte, Int16, UInt16, Int32, UInt32, Int64, UInt64, IntPtr, UIntPtr, Char, Double, and Single.
            if (type.IsPrimitive)
            {
                if (type == typeof(Byte) ||
                    type == typeof(SByte) ||
                    type == typeof(Int16) ||
                    type == typeof(UInt16) ||
                    type == typeof(Int32)
                    )
                {
                    return "{0}->IsInt32()";
                }
                else if (type == typeof(UInt32))
                {
                    return "{0}->IsUint32()";
                }
                else if (type == typeof(Int64) ||
                    type == typeof(UInt64))
                {
                    return "{0}->IsNumber()";
                }
                else if (type == typeof(Double) ||
                    type == typeof(Single) ||
                    type == typeof(IntPtr) ||
                    type == typeof(UIntPtr))
                {
                    return "{0}->IsNumber()";
                }
                else if (type == typeof(Boolean))
                {
                    return "{0}->IsBoolean()";
                }
                else if (type == typeof(Char))
                {
                    return "{0}->IsString()";
                }
            }

            if (type.IsGenericType && type.GetGenericTypeDefinition() == typeof(Nullable<>))
            {
                return TypeCheck(type.GetGenericArguments()[0], TX.MainModel.Types.ContainsKey(type.GetGenericArguments()[0]));
            }

            if (type == typeof(Guid))
            {
                return "NodeRT::Utils::IsGuid({0})";
            }

            if (type.FullName == "Windows.UI.Color")
            {
                return "NodeRT::Utils::IsColor({0})";
            }

            if (type.FullName == "Windows.Foundation.Point")
            {
                return "NodeRT::Utils::IsPoint({0})";
            }

            if (type.FullName == "Windows.Foundation.Size")
            {
                return "NodeRT::Utils::IsSize({0})";
            }

            if (type.FullName == "Windows.Foundation.Rect")
            {
                return "NodeRT::Utils::IsRect({0})";
            }

            if (type.IsEnum)
            {
                return "{0}->IsInt32()";
            }

            if (type == typeof(String))
            {
                return "{0}->IsString()";
            }

            if (type == typeof(DateTimeOffset) || type == typeof(DateTime))
            {
                return "{0}->IsDate()";
            }

            if (type == typeof(TimeSpan))
            {
                return "{0}->IsNumber()";
            }

            // this if clause should come after IsEnum, since an enum will also return true for IsValueType
            if (type.IsValueType && !(type.IsGenericType && type.FullName.StartsWith("System.Collections.Generic.KeyValuePair`2")))
            {
                return "Is" + type.Name + "JsObject({0})";
            }

            return "NodeRT::Utils::IsWinRtWrapperOf<" + TX.ToWinRT(type, true) + ">({0})";
        }

        public static string ToOutParameterName(Type type)
        {
            // The primitive types are Boolean, Byte, SByte, Int16, UInt16, Int32, UInt32, Int64, UInt64, IntPtr, UIntPtr, Char, Double, and Single.
            if (type.IsPrimitive)
            {
                if (type == typeof(Byte) || type == typeof(SByte) || type == typeof(Int16) || 
                    type == typeof(UInt16) || type == typeof(Int32) || type == typeof(UInt32) || 
                    type == typeof(Int64) || type == typeof(IntPtr) || type == typeof(UIntPtr) || 
                    type == typeof(Single) || type == typeof(Double))
                {
                    return "number";
                }
                else if (type == typeof(Boolean))
                {
                    return "boolean";
                }
                else if (type == typeof(Char))
                {
                    return "string";
                }
            }

            if (type == typeof(String))
            {
                return "string";
            }

            if (type == typeof(DateTimeOffset) || type == typeof(DateTime))
            {
                return "date";

            }

            return TX.Uncap(type.Name);
        }
    }
}
