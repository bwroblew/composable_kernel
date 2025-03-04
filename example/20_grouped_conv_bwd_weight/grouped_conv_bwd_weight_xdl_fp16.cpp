// SPDX-License-Identifier: MIT
// Copyright (c) 2018-2023, Advanced Micro Devices, Inc. All rights reserved.

#include "common.hpp"

#include "ck/tensor_operation/gpu/device/impl/device_grouped_conv_bwd_weight_gnwc_gkxc_gnwk_xdl_cshuffle.hpp"

using InDataType  = F16;
using WeiDataType = F16;
using OutDataType = F16;
using AccDataType = F32;

using InElementOp  = PassThrough;
using WeiElementOp = PassThrough;
using OutElementOp = PassThrough;

template <ck::index_t NDimSpatial>
using DeviceConvBwdWeightInstance =
    ck::tensor_operation::device::DeviceGroupedConvBwdWeightGnwcGkxcGnwk_Xdl_CShuffle<
        NDimSpatial,          // NDimSpatial
        InDataType,           // InDataType
        WeiDataType,          // WeiDataType
        OutDataType,          // OutDataType
        AccDataType,          // AccDataType
        InElementOp,          // InElementwiseOperation
        WeiElementOp,         // WeiElementwiseOperation
        OutElementOp,         // OutElementwiseOperation
        ConvBwdWeightDefault, // ConvolutionBackwardWeightSpecialization
        256,                  // BlockSize
        128,                  // MPerBlock
        128,                  // NPerBlock
        4,                    // K0PerBlock
        8,                    // K1
        32,                   // MPerXdl
        32,                   // NPerXdl
        2,                    // MXdlPerWave
        2,                    // NXdlPerWave
        S<1, 4, 16, 4>,       // ABlockTransferThreadClusterLengths_K0_M_K1
        S<0, 3, 1, 2>,        // ABlockTransferThreadClusterArrangeOrder
        S<0, 2, 1, 3>,        // ABlockTransferSrcAccessOrder
        2,                    // ABlockTransferSrcVectorDim
        8,                    // ABlockTransferSrcScalarPerVector
        2,                    // ABlockTransferDstScalarPerVector_K1
        true,                 // ABlockLdsAddExtraM
        S<1, 4, 16, 4>,       // BBlockTransferThreadClusterLengths_K0_N_K1
        S<0, 3, 1, 2>,        // BBlockTransferThreadClusterArrangeOrder
        S<0, 2, 1, 3>,        // BBlockTransferSrcAccessOrder
        2,                    // BBlockTransferSrcVectorDim
        8,                    // BBlockTransferSrcScalarPerVector
        2,                    // BBlockTransferDstScalarPerVector_K1
        true,                 // BBlockLdsAddExtraN
        1,                    // CShuffleMXdlPerWavePerShuffle
        1,                    // CShuffleNXdlPerWavePerShuffle
        S<1, 32, 1, 4>,       // CBlockTransferClusterLengths_MBlock_MPerBlock_NBlock_NPerBlock
        128 / (sizeof(WeiDataType) * CHAR_BIT)>; // CBlockTransferScalarPerVector_NWaveNPerXdl

#include "run_grouped_conv_bwd_weight_example.inc"

int main(int argc, char* argv[]) { return !run_grouped_conv_bwd_weight_example(argc, argv); }
