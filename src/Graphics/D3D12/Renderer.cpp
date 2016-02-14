// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/D3D12/Renderer.h"

using Microsoft::WRL::ComPtr;
using rainbow::d3d12::Renderer;

namespace
{
    void GetHardwareAdapter(IDXGIFactory4* pFactory, IDXGIAdapter1** ppAdapter)
    {
        *ppAdapter = nullptr;
        for (UINT adapterIndex = 0;; ++adapterIndex)
        {
            IDXGIAdapter1* pAdapter = nullptr;
            if (DXGI_ERROR_NOT_FOUND ==
                pFactory->EnumAdapters1(adapterIndex, &pAdapter))
            {
                // No more adapters to enumerate.
                break;
            }

            // Check to see if the adapter supports Direct3D 12, but don't
            // create the actual device yet.
            if (SUCCEEDED(D3D12CreateDevice(pAdapter,
                                            D3D_FEATURE_LEVEL_11_0,
                                            _uuidof(ID3D12Device),
                                            nullptr)))
            {
                *ppAdapter = pAdapter;
                return;
            }

            pAdapter->Release();
        }
    }
}

bool Renderer::init(HWND hWnd)
{
#ifndef NDEBUG
    {
        ComPtr<ID3D12Debug> debug_interface;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_interface))))
            debug_interface->EnableDebugLayer();
    }
#endif

    ComPtr<IDXGIFactory4> factory;
    if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&factory))))
        return false;

    ComPtr<IDXGIAdapter1> adapter;
    GetHardwareAdapter(factory.Get(), &adapter);
    if (FAILED(D3D12CreateDevice(
            adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device_))))
    {
        return false;
    }

    D3D12_COMMAND_QUEUE_DESC queue_desc{D3D12_COMMAND_LIST_TYPE_DIRECT,
                                        D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
                                        D3D12_COMMAND_QUEUE_FLAG_NONE,
                                        0};
    device_->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&command_queue_));

    DXGI_SWAP_CHAIN_DESC1 swap_chain_desc{0,
                                          0,
                                          DXGI_FORMAT_R8G8B8A8_UNORM,
                                          FALSE,
                                          DXGI_SAMPLE_DESC{1, 0},
                                          DXGI_USAGE_RENDER_TARGET_OUTPUT,
                                          2,
                                          DXGI_SCALING_ASPECT_RATIO_STRETCH,
                                          DXGI_SWAP_EFFECT_FLIP_DISCARD,
                                          DXGI_ALPHA_MODE_PREMULTIPLIED,
                                          0};
    ComPtr<IDXGISwapChain1> swap_chain;
    if (FAILED(factory->CreateSwapChainForHwnd(command_queue_.Get(),
                                               hWnd,
                                               &swap_chain_desc,
                                               nullptr,
                                               nullptr,
                                               &swap_chain)))
    {
        return false;
    }

    if (FAILED(factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_PRINT_SCREEN)))
        return false;

    if (FAILED(swap_chain.As(&swap_chain_)))
        return false;

    frame_index_ = swap_chain_->GetCurrentBackBufferIndex();

    return true;
}
