// Copyright (c) Microsoft Corporation
// All rights reserved. 
//
// Licensed under the Apache License, Version 2.0 (the ""License""); you may not use this file except in compliance with the License. You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0 
//
// THIS CODE IS PROVIDED ON AN  *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED WARRANTIES OR CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE, MERCHANTABLITY OR NON-INFRINGEMENT. 
//
// See the Apache Version 2.0 License for specific language governing permissions and limitations under the License.

#pragma once

#include <wrl.h>
#include <wrl/implements.h>
#include <windows.storage.streams.h>
#include <robuffer.h>
#include <vector>

namespace NodeRT { namespace Buffer { namespace Utils {

// taken from http://stackoverflow.com/a/12125386/1060807
class NativeBuffer : 
  public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::RuntimeClassType::WinRtClassicComMix>,
  ABI::Windows::Storage::Streams::IBuffer,
  Windows::Storage::Streams::IBufferByteAccess>
{
public:
  virtual ~NativeBuffer()
  {
  }

  STDMETHODIMP RuntimeClassInitialize(byte *buffer, UINT32 totalSize)
  {
    m_length = totalSize;
    m_buffer = buffer;

    return S_OK;
  }

  STDMETHODIMP Buffer(byte **value)
  {
    *value = m_buffer;

    return S_OK;
  }

  STDMETHODIMP get_Capacity(UINT32 *value)
  {
    *value = m_length;

    return S_OK;
  }

  STDMETHODIMP get_Length(UINT32 *value)
  {
    *value = m_length;

    return S_OK;
  }

  STDMETHODIMP put_Length(UINT32 value)
  {
    m_length = value;

    return S_OK;
  }

private:
  UINT32 m_length;
  byte *m_buffer;
};

}}}