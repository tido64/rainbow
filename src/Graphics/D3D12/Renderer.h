// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_D3D12_RENDERER_H_
#define GRAPHICS_D3D12_RENDERER_H_

#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl.h>

namespace rainbow { namespace d3d12
{
    class Renderer
    {
    public:
        bool init(HWND hWnd);

    private:
        Microsoft::WRL::ComPtr<ID3D12Device> device_;
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> command_queue_;
        Microsoft::WRL::ComPtr<IDXGISwapChain3> swap_chain_;
        unsigned int frame_index_ = 0;
    };
}}  // namespace rainbow::d3d12

#endif
