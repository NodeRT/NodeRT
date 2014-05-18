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
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace NodeRTLib
{
    // a helper utility class used for fetching value types which are used by the types in the namespace
    // but are not part of the current namespace
    class ReflectorValueTypesHelper
    {
        public static bool IsValueTypeNoEnumOrPrimitve(Type type)
        {
            return type.IsValueType && !type.IsEnum && !type.IsPrimitive && type.Namespace != "System";
        }

        public static void TestAndAddValueTypeRecursive(Type type, List<Type> valueTypes, String declaringTypeNamespace)
        {
            if (type.IsGenericType)
            {
                if (type.GetGenericArguments() != null)
                {
                    foreach (var genericArg in type.GetGenericArguments())
                    {
                        TestAndAddValueTypeRecursive(genericArg, valueTypes, declaringTypeNamespace);
                    }
                }
            }
            else if (IsValueTypeNoEnumOrPrimitve(type) && !valueTypes.Contains(type) && 
                type.Namespace != "System" && type.Namespace != declaringTypeNamespace)
            {
                valueTypes.Add(type);
            }
        }

        public static void GetExternalReferencedValueTypes(EventInfo info, List<Type> types)
        {
            var invokeMethodInfo = info.EventHandlerType.GetMethods().Where((methodInfo) => { return (methodInfo.Name == "Invoke"); }).First();

            GetExternalReferencedValueTypes(invokeMethodInfo.GetParameters(), info.DeclaringType.Namespace, types);
        }

        public static void GetExternalReferencedValueTypes(ParameterInfo[] info, string nameSpace, List<Type> types)
        {
            foreach (var param in info)
            {
                TestAndAddValueTypeRecursive(param.ParameterType, types, nameSpace);
            }
        }

        public static void GetExternalReferencedValueTypes(MethodInfo info, List<Type> types)
        {
            TestAndAddValueTypeRecursive(info.ReturnType, types, info.DeclaringType.Namespace);

            GetExternalReferencedValueTypes(info.GetParameters(), info.DeclaringType.Namespace, types);
        }

        public static void GetExternalReferencedValueTypes(PropertyInfo info, List<Type> types)
        {
            TestAndAddValueTypeRecursive(info.PropertyType, types, info.DeclaringType.Namespace);
        }

        public static void GetExternalReferencedValueTypesFromMethods(dynamic[] methods, List<Type> types)
        {
            foreach (dynamic method in methods)
            {
                foreach (MethodInfo mi in method.Overloads)
                {
                    GetExternalReferencedValueTypes(mi, types);
                }
            }
        }

        public static void GetExternalReferencedValueTypesForType(dynamic typeDefinition, List<Type> externalValueTypes)
        {            
            foreach (ConstructorInfo ci in typeDefinition.Type.GetConstructors())
            {
                GetExternalReferencedValueTypes(ci.GetParameters(), typeDefinition.Type.Namespace, externalValueTypes);
            }

            foreach (PropertyInfo pi in typeDefinition.MemberProperties)
            {
                GetExternalReferencedValueTypes(pi, externalValueTypes);
            }

            foreach (PropertyInfo pi in typeDefinition.StaticProperties)
            {
                GetExternalReferencedValueTypes(pi, externalValueTypes);
            }

            foreach (dynamic ev in typeDefinition.Events)
            {
                EventInfo ei = ev.EventInfo;
                GetExternalReferencedValueTypes(ei, externalValueTypes);
            }

            GetExternalReferencedValueTypesFromMethods(typeDefinition.MemberAsyncMethods, externalValueTypes);
            GetExternalReferencedValueTypesFromMethods(typeDefinition.StaticAsyncMethods, externalValueTypes);
            GetExternalReferencedValueTypesFromMethods(typeDefinition.MemberSyncMethods, externalValueTypes);
            GetExternalReferencedValueTypesFromMethods(typeDefinition.StaticSyncMethods, externalValueTypes);
        }
    }
}
