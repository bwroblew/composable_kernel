// SPDX-License-Identifier: MIT
// Copyright (c) 2018-2023, Advanced Micro Devices, Inc. All rights reserved.

#include <cstdlib>

#include "ck/ck.hpp"
#include "ck/library/tensor_operation_instance/add_device_operation_instance.hpp"
#include "ck/tensor_operation/gpu/device/device_gemm_xdl_waveletmodel_cshuffle.hpp"
#include "ck/tensor_operation/gpu/device/gemm_specialization.hpp"
#include "ck/tensor_operation/gpu/device/tensor_layout.hpp"
#include "ck/tensor_operation/gpu/element/element_wise_operation.hpp"

#include "gemm_wavelet_f16_tn_instance.hpp"

namespace ck {
namespace tensor_operation {
namespace device {
namespace instance {

using F16 = ck::half_t;
using F32 = float;

using Row = ck::tensor_layout::gemm::RowMajor;
using Col = ck::tensor_layout::gemm::ColumnMajor;

template <ck::index_t... Is>
using S = ck::Sequence<Is...>;

using PassThrough = ck::tensor_operation::element_wise::PassThrough;

static constexpr auto GemmDefault = ck::tensor_operation::device::GemmSpecialization::Default;

using gemm_f16_tn_256x256 = std::tuple<
    // clang-format off
    //#####################             | ALayout| BLayout| CLayout| AData| BData| AccData| CShuffle| CData|           A|           B|           C|           GEMM| NumGemmK| ABBlockTransfer|       BlockGemm|  MPer|  NPer|  KPer| AK1| BK1| MPer| NPer| MXdl| NXdl|  ABlockTransfer| ABlockTransfer| ABlockTransfer| ABlockTransfer| ABlockTransfer| ABlockTransfer| ABlockLds|  BBlockTransfer| BBlockTransfer| BBlockTransfer| BlockTransfer| BBlockTransfer| BBlockTransfer| BBlockLds|    CShuffle|    CShuffle| CBlockTransferClusterLengths|  CBlockTransfer|
    //#####################             |        |        |        |  Type|  Type|    Type| DataType|  Type| Elementwise| Elementwise| Elementwise| Specialization| Prefetch| ThreadGroupSize| ThreadGroupSize| Block| Block| Block|    |    |  XDL|  XDL|  Per|  Per|   ThreadCluster|  ThreadCluster| SrcAccessOrder|   SrcVectorDim|      SrcScalar|      DstScalar| AddExtraM|   ThreadCluster|  ThreadCluster| SrcAccessOrder|  SrcVectorDim|      SrcScalar|      DstScalar| AddExtraN| MXdlPerWave| NXdlPerWave|         _MBlock_MWaveMPerXdl| ScalarPerVector|
    //#####################             |        |        |        |      |      |        |         |      |   Operation|   Operation|   Operation|               |    Stage|                |                |      |      |      |    |    |     |     | Wave| Wave| Lengths_K0_M_K1|   ArrangeOrder|               |               |      PerVector|   PerVector_K1|          | Lengths_K0_N_K1|   ArrangeOrder|               |              |      PerVector|   PerVector_K1|          |  PerShuffle|  PerShuffle|         _NBlock_NWaveNPerXdl|   _NWaveNPerXdl|
    //#####################             |        |        |        |      |      |        |         |      |            |            |            |               |         |                |                |      |      |      |    |    |     |     |     |     |                |               |               |               |               |               |          |                |               |               |              |               |               |          |            |            |                             |                |
    DeviceGemm_Xdl_WaveletModel_CShuffle<     Row,     Col,     Row,   F16,   F16,     F32,     F16,    F16, PassThrough, PassThrough, PassThrough,    GemmDefault,        1,             256,             256,   256,   256,    32,   8,   8,   32,   32,    4,    4,     S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              8,              8,         1,     S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,             2,              8,              8,         1,           1,           1,               S<1, 32, 1, 8>,              8>
    // clang-format on
    >;

using gemm_f16_tn_256x128 = std::tuple<
    // clang-format off
    //#####################             | ALayout| BLayout| CLayout| AData| BData| AccData| CShuffle| CData|           A|           B|           C|           GEMM| NumGemmK| ABBlockTransfer|       BlockGemm|  MPer|  NPer|  KPer| AK1| BK1| MPer| NPer| MXdl| NXdl|  ABlockTransfer| ABlockTransfer| ABlockTransfer| ABlockTransfer| ABlockTransfer| ABlockTransfer| ABlockLds|  BBlockTransfer| BBlockTransfer| BBlockTransfer| BlockTransfer| BBlockTransfer| BBlockTransfer| BBlockLds|    CShuffle|    CShuffle| CBlockTransferClusterLengths|  CBlockTransfer|
    //#####################             |        |        |        |  Type|  Type|    Type| DataType|  Type| Elementwise| Elementwise| Elementwise| Specialization| Prefetch| ThreadGroupSize| ThreadGroupSize| Block| Block| Block|    |    |  XDL|  XDL|  Per|  Per|   ThreadCluster|  ThreadCluster| SrcAccessOrder|   SrcVectorDim|      SrcScalar|      DstScalar| AddExtraM|   ThreadCluster|  ThreadCluster| SrcAccessOrder|  SrcVectorDim|      SrcScalar|      DstScalar| AddExtraN| MXdlPerWave| NXdlPerWave|         _MBlock_MWaveMPerXdl| ScalarPerVector|
    //#####################             |        |        |        |      |      |        |         |      |   Operation|   Operation|   Operation|               |    Stage|                |                |      |      |      |    |    |     |     | Wave| Wave| Lengths_K0_M_K1|   ArrangeOrder|               |               |      PerVector|   PerVector_K1|          | Lengths_K0_N_K1|   ArrangeOrder|               |              |      PerVector|   PerVector_K1|          |  PerShuffle|  PerShuffle|         _NBlock_NWaveNPerXdl|   _NWaveNPerXdl|
    //#####################             |        |        |        |      |      |        |         |      |            |            |            |               |         |                |                |      |      |      |    |    |     |     |     |     |                |               |               |               |               |               |          |                |               |               |              |               |               |          |            |            |                             |                |
    DeviceGemm_Xdl_WaveletModel_CShuffle<     Row,     Col,     Row,   F16,   F16,     F32,      F16,   F16, PassThrough, PassThrough, PassThrough,    GemmDefault,        1,             256,             256,   256,   128,    32,   8,   8,   32,   32,    4,    2,     S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              8,              8,         1,     S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,             2,              8,              8,         1,           1,           1,               S<1, 32, 1, 8>,              8>
    // clang-format on
    >;

using gemm_f16_tn_128x128 = std::tuple<
    // clang-format off
    //#####################             | ALayout| BLayout| CLayout| AData| BData| AccData| CShuffle| CData|           A|           B|           C|           GEMM| NumGemmK| ABBlockTransfer|       BlockGemm|  MPer|  NPer|  KPer| AK1| BK1| MPer| NPer| MXdl| NXdl|  ABlockTransfer| ABlockTransfer| ABlockTransfer| ABlockTransfer| ABlockTransfer| ABlockTransfer| ABlockLds|  BBlockTransfer| BBlockTransfer| BBlockTransfer| BlockTransfer| BBlockTransfer| BBlockTransfer| BBlockLds|    CShuffle|    CShuffle| CBlockTransferClusterLengths|  CBlockTransfer|
    //#####################             |        |        |        |  Type|  Type|    Type| DataType|  Type| Elementwise| Elementwise| Elementwise| Specialization| Prefetch| ThreadGroupSize| ThreadGroupSize| Block| Block| Block|    |    |  XDL|  XDL|  Per|  Per|   ThreadCluster|  ThreadCluster| SrcAccessOrder|   SrcVectorDim|      SrcScalar|      DstScalar| AddExtraM|   ThreadCluster|  ThreadCluster| SrcAccessOrder|  SrcVectorDim|      SrcScalar|      DstScalar| AddExtraN| MXdlPerWave| NXdlPerWave|         _MBlock_MWaveMPerXdl| ScalarPerVector|
    //#####################             |        |        |        |      |      |        |         |      |   Operation|   Operation|   Operation|               |    Stage|                |                |      |      |      |    |    |     |     | Wave| Wave| Lengths_K0_M_K1|   ArrangeOrder|               |               |      PerVector|   PerVector_K1|          | Lengths_K0_N_K1|   ArrangeOrder|               |              |      PerVector|   PerVector_K1|          |  PerShuffle|  PerShuffle|         _NBlock_NWaveNPerXdl|   _NWaveNPerXdl|
    //#####################             |        |        |        |      |      |        |         |      |            |            |            |               |         |                |                |      |      |      |    |    |     |     |     |     |                |               |               |               |               |               |          |                |               |               |              |               |               |          |            |            |                             |                |
    DeviceGemm_Xdl_WaveletModel_CShuffle<     Row,     Col,     Row,   F16,   F16,     F32,      F16,   F16, PassThrough, PassThrough, PassThrough,    GemmDefault,        1,             256,             256,   128,   128,    32,   8,   8,   32,   32,    2,    2,     S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              8,              8,         1,     S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,             2,              8,              8,         1,           1,           1,               S<1, 32, 1, 8>,              8>
    // clang-format on
    >;

using gemm_f16_tn_128x64 = std::tuple<
    // clang-format off
    //#####################             | ALayout| BLayout| CLayout| AData| BData| AccData| CShuffle| CData|           A|           B|           C|           GEMM| NumGemmK| ABBlockTransfer|       BlockGemm|  MPer|  NPer|  KPer| AK1| BK1| MPer| NPer| MXdl| NXdl|  ABlockTransfer| ABlockTransfer| ABlockTransfer| ABlockTransfer| ABlockTransfer| ABlockTransfer| ABlockLds|  BBlockTransfer| BBlockTransfer| BBlockTransfer| BlockTransfer| BBlockTransfer| BBlockTransfer| BBlockLds|    CShuffle|    CShuffle| CBlockTransferClusterLengths|  CBlockTransfer|
    //#####################             |        |        |        |  Type|  Type|    Type| DataType|  Type| Elementwise| Elementwise| Elementwise| Specialization| Prefetch| ThreadGroupSize| ThreadGroupSize| Block| Block| Block|    |    |  XDL|  XDL|  Per|  Per|   ThreadCluster|  ThreadCluster| SrcAccessOrder|   SrcVectorDim|      SrcScalar|      DstScalar| AddExtraM|   ThreadCluster|  ThreadCluster| SrcAccessOrder|  SrcVectorDim|      SrcScalar|      DstScalar| AddExtraN| MXdlPerWave| NXdlPerWave|         _MBlock_MWaveMPerXdl| ScalarPerVector|
    //#####################             |        |        |        |      |      |        |         |      |   Operation|   Operation|   Operation|               |    Stage|                |                |      |      |      |    |    |     |     | Wave| Wave| Lengths_K0_M_K1|   ArrangeOrder|               |               |      PerVector|   PerVector_K1|          | Lengths_K0_N_K1|   ArrangeOrder|               |              |      PerVector|   PerVector_K1|          |  PerShuffle|  PerShuffle|         _NBlock_NWaveNPerXdl|   _NWaveNPerXdl|
    //#####################             |        |        |        |      |      |        |         |      |            |            |            |               |         |                |                |      |      |      |    |    |     |     |     |     |                |               |               |               |               |               |          |                |               |               |              |               |               |          |            |            |                             |                |
    DeviceGemm_Xdl_WaveletModel_CShuffle<     Row,     Col,     Row,   F16,   F16,     F32,      F16,   F16, PassThrough, PassThrough, PassThrough,    GemmDefault,        1,             256,             256,   128,    64,    32,   8,   8,   32,   32,    2,    1,     S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              8,              8,         1,     S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,             2,              8,              8,         1,           1,           1,               S<1, 32, 1, 8>,              8>
    // clang-format on
    >;

void add_gemm_wavelet_f16_tn_256x256(std::vector<std::unique_ptr<BaseOperator>>& instances)
{
    add_device_operation_instances(instances, gemm_f16_tn_256x256{});
}

void add_gemm_wavelet_f16_tn_256x128(std::vector<std::unique_ptr<BaseOperator>>& instances)
{
    add_device_operation_instances(instances, gemm_f16_tn_256x128{});
}

void add_gemm_wavelet_f16_tn_128x128(std::vector<std::unique_ptr<BaseOperator>>& instances)
{
    add_device_operation_instances(instances, gemm_f16_tn_128x128{});
}

void add_gemm_wavelet_f16_tn_128x64(std::vector<std::unique_ptr<BaseOperator>>& instances)
{
    add_device_operation_instances(instances, gemm_f16_tn_128x64{});
}

} // namespace instance
} // namespace device
} // namespace tensor_operation
} // namespace ck
