// SPDX-License-Identifier: MIT
// Copyright (c) 2018-2023, Advanced Micro Devices, Inc. All rights reserved.

#include <cstdlib>

#include "ck/ck.hpp"
#include "ck/tensor_operation/gpu/device/tensor_layout.hpp"
#include "ck/tensor_operation/gpu/device/gemm_specialization.hpp"
#include "ck/tensor_operation/gpu/device/impl/device_gemm_xdl_cshuffle.hpp"
#include "ck/library/tensor_operation_instance/add_device_operation_instance.hpp"

namespace ck {
namespace tensor_operation {
namespace device {
namespace instance {

using F32 = float;

using Row = ck::tensor_layout::gemm::RowMajor;
using Col = ck::tensor_layout::gemm::ColumnMajor;

template <ck::index_t... Is>
using S = ck::Sequence<Is...>;

using PassThrough = ck::tensor_operation::element_wise::PassThrough;

static constexpr auto GemmDefault = ck::tensor_operation::device::GemmSpecialization::Default;

// Compilation parameters for a[k, m] * b[n, k] = c[m, n]
using device_gemm_xdl_c_shuffle_i8_i8_i8_km_nk_mn_instances =
    std::tuple<
        // clang-format off
        //#####################| ALayout| BLayout| CLayout|  AData|  BData|  CData| AccData| CShuffle|           A|           B|           C|           GEMM| NumGemmK| Block|  MPer|  NPer|  KPer| AK1| BK1| MPer| NPer| MXdl| NXdl|  ABlockTransfer| ABlockTransfer| ABlockTransfer| ABlockTransfer| ABlockTransfer| ABlockTransfer| ABlockLds|  BBlockTransfer| BBlockTransfer| BBlockTransfer| BlockTransfer| BBlockTransfer| BBlockTransfer| BBlockLds|    CShuffle|    CShuffle| CBlockTransferClusterLengths|  CBlockTransfer|
        //#####################|        |        |        |   Type|   Type|   Type|    Type| DataType| Elementwise| Elementwise| Elementwise| Specialization| Prefetch|  Size| Block| Block| Block|    |    |  XDL|  XDL|  Per|  Per|   ThreadCluster|  ThreadCluster| SrcAccessOrder|   SrcVectorDim|      SrcScalar|      DstScalar| AddExtraM|   ThreadCluster|  ThreadCluster| SrcAccessOrder|  SrcVectorDim|      SrcScalar|      DstScalar| AddExtraN| MXdlPerWave| NXdlPerWave|         _MBlock_MWaveMPerXdl| ScalarPerVector|
        //#####################|        |        |        |       |       |       |        |         |   Operation|   Operation|   Operation|               |    Stage|      |      |      |      |    |    |     |     | Wave| Wave| Lengths_K0_M_K1|   ArrangeOrder|               |               |      PerVector|   PerVector_K1|          | Lengths_K0_N_K1|   ArrangeOrder|               |              |      PerVector|   PerVector_K1|          |  PerShuffle|  PerShuffle|         _NBlock_NWaveNPerXdl|   _NWaveNPerXdl|
        //#####################|        |        |        |       |       |       |        |         |            |            |            |               |         |      |      |      |      |    |    |     |     |     |     |                |               |               |               |               |               |          |                |               |               |              |               |               |          |            |            |                             |                |
        DeviceGemm_Xdl_CShuffle<     Col,     Col,     Row, int8_t, int8_t, int8_t, int32_t,  int32_t, PassThrough, PassThrough, PassThrough,    GemmDefault,        1,   256,   256,   128,    64,   4,  16,   32,   32,    4,    2,     S<4, 64, 1>,     S<0, 2, 1>,     S<0, 2, 1>,              1,              4,              4,         0,     S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,             2,             16,             16,         1,           1,           1,               S<1, 64, 1, 4>,             16>,
        DeviceGemm_Xdl_CShuffle<     Col,     Col,     Row, int8_t, int8_t, int8_t, int32_t,  int32_t, PassThrough, PassThrough, PassThrough,    GemmDefault,        1,   256,   256,   128,    64,  16,  16,   32,   32,    4,    2,     S<4, 64, 1>,     S<0, 2, 1>,     S<0, 2, 1>,              1,              4,             16,         1,     S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,             2,             16,             16,         1,           1,           1,               S<1, 64, 1, 4>,             16>,
        DeviceGemm_Xdl_CShuffle<     Col,     Col,     Row, int8_t, int8_t, int8_t, int32_t,  int32_t, PassThrough, PassThrough, PassThrough,    GemmDefault,        1,   256,   128,   256,    64,   4,  16,   32,   32,    2,    4,     S<8, 32, 1>,     S<0, 2, 1>,     S<0, 2, 1>,              1,              4,              4,         0,     S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,             2,             16,             16,         1,           1,           1,               S<1, 64, 1, 4>,             16>,
        DeviceGemm_Xdl_CShuffle<     Col,     Col,     Row, int8_t, int8_t, int8_t, int32_t,  int32_t, PassThrough, PassThrough, PassThrough,    GemmDefault,        1,   256,   128,   256,    64,  16,  16,   32,   32,    2,    4,     S<4, 64, 1>,     S<0, 2, 1>,     S<0, 2, 1>,              1,              2,             16,         1,     S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,             2,             16,             16,         1,           1,           1,               S<1, 64, 1, 4>,             16>,
        DeviceGemm_Xdl_CShuffle<     Col,     Col,     Row, int8_t, int8_t, int8_t, int32_t,  int32_t, PassThrough, PassThrough, PassThrough,    GemmDefault,        1,   128,   128,   128,    64,   4,  16,   32,   32,    4,    2,     S<4, 32, 1>,     S<0, 2, 1>,     S<0, 2, 1>,              1,              4,              4,         0,     S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,             2,             16,             16,         1,           1,           1,               S<1, 32, 1, 4>,             16>,
        DeviceGemm_Xdl_CShuffle<     Col,     Col,     Row, int8_t, int8_t, int8_t, int32_t,  int32_t, PassThrough, PassThrough, PassThrough,    GemmDefault,        1,   128,   128,   128,    64,  16,  16,   32,   32,    4,    2,     S<4, 32, 1>,     S<0, 2, 1>,     S<0, 2, 1>,              1,              4,             16,         1,     S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,             2,             16,             16,         1,           1,           1,               S<1, 32, 1, 4>,             16>,
        DeviceGemm_Xdl_CShuffle<     Col,     Col,     Row, int8_t, int8_t, int8_t, int32_t,  int32_t, PassThrough, PassThrough, PassThrough,    GemmDefault,        1,   256,   128,   128,    64,   4,  16,   32,   32,    2,    2,     S<8, 32, 1>,     S<0, 2, 1>,     S<0, 2, 1>,              1,              4,              4,         0,     S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,             2,             16,             16,         1,           1,           1,               S<1, 64, 1, 4>,             16>,
        DeviceGemm_Xdl_CShuffle<     Col,     Col,     Row, int8_t, int8_t, int8_t, int32_t,  int32_t, PassThrough, PassThrough, PassThrough,    GemmDefault,        1,   256,   128,   128,    64,  16,  16,   32,   32,    2,    2,     S<4, 64, 1>,     S<0, 2, 1>,     S<0, 2, 1>,              1,              2,             16,         1,     S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,             2,             16,             16,         1,           1,           1,               S<1, 64, 1, 4>,             16>,
        DeviceGemm_Xdl_CShuffle<     Col,     Col,     Row, int8_t, int8_t, int8_t, int32_t,  int32_t, PassThrough, PassThrough, PassThrough,    GemmDefault,        1,   128,   128,    64,    64,   4,  16,   32,   32,    2,    2,     S<4, 32, 1>,     S<0, 2, 1>,     S<0, 2, 1>,              1,              4,              4,         0,     S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,             2,             16,             16,         1,           1,           1,               S<1, 64, 1, 2>,             16>,
        DeviceGemm_Xdl_CShuffle<     Col,     Col,     Row, int8_t, int8_t, int8_t, int32_t,  int32_t, PassThrough, PassThrough, PassThrough,    GemmDefault,        1,   128,   128,    64,    64,  16,  16,   32,   32,    2,    2,     S<4, 32, 1>,     S<0, 2, 1>,     S<0, 2, 1>,              1,              4,             16,         1,     S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,             2,             16,             16,         1,           1,           1,               S<1, 64, 1, 2>,             16>,
        DeviceGemm_Xdl_CShuffle<     Col,     Col,     Row, int8_t, int8_t, int8_t, int32_t,  int32_t, PassThrough, PassThrough, PassThrough,    GemmDefault,        1,   128,    64,   128,    64,   4,  16,   32,   32,    2,    2,     S<8, 16, 1>,     S<0, 2, 1>,     S<0, 2, 1>,              1,              4,              4,         0,     S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,             2,             16,             16,         1,           1,           1,               S<1, 32, 1, 4>,             16>,
        DeviceGemm_Xdl_CShuffle<     Col,     Col,     Row, int8_t, int8_t, int8_t, int32_t,  int32_t, PassThrough, PassThrough, PassThrough,    GemmDefault,        1,   128,    64,   128,    64,  16,  16,   32,   32,    2,    2,     S<4, 32, 1>,     S<0, 2, 1>,     S<0, 2, 1>,              1,              2,             16,         1,     S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,             2,             16,             16,         1,           1,           1,               S<1, 32, 1, 4>,             16>,
        DeviceGemm_Xdl_CShuffle<     Col,     Col,     Row, int8_t, int8_t, int8_t, int32_t,  int32_t, PassThrough, PassThrough, PassThrough,    GemmDefault,        1,   256,   128,    64,    64,   4,  16,   32,   32,    2,    1,     S<8, 32, 1>,     S<0, 2, 1>,     S<0, 2, 1>,              1,              4,              4,         0,     S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,             2,             16,             16,         1,           1,           1,               S<1, 64, 1, 4>,             16>,
        DeviceGemm_Xdl_CShuffle<     Col,     Col,     Row, int8_t, int8_t, int8_t, int32_t,  int32_t, PassThrough, PassThrough, PassThrough,    GemmDefault,        1,   256,   128,    64,    64,  16,  16,   32,   32,    2,    1,     S<4, 64, 1>,     S<0, 2, 1>,     S<0, 2, 1>,              1,              2,             16,         1,     S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,             2,             16,             16,         1,           1,           1,               S<1, 64, 1, 4>,             16>,
        DeviceGemm_Xdl_CShuffle<     Col,     Col,     Row, int8_t, int8_t, int8_t, int32_t,  int32_t, PassThrough, PassThrough, PassThrough,    GemmDefault,        1,   256,    64,   128,    64,   4,  16,   32,   32,    1,    2,     S<16,16, 1>,     S<0, 2, 1>,     S<0, 2, 1>,              1,              4,              4,         0,     S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,             2,             16,             16,         1,           1,           1,               S<1, 64, 1, 4>,             16>,
        DeviceGemm_Xdl_CShuffle<     Col,     Col,     Row, int8_t, int8_t, int8_t, int32_t,  int32_t, PassThrough, PassThrough, PassThrough,    GemmDefault,        1,   256,    64,   128,    64,  16,  16,   32,   32,    1,    2,     S<4, 64, 1>,     S<0, 2, 1>,     S<0, 2, 1>,              1,              1,             16,         1,     S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,             2,             16,             16,         1,           1,           1,               S<1, 64, 1, 4>,             16>
        // clang-format on
        >;

void add_device_gemm_xdl_c_shuffle_i8_i8_i8_km_nk_mn_instances(
    std::vector<std::unique_ptr<
        DeviceGemm<Col, Col, Row, int8_t, int8_t, int8_t, PassThrough, PassThrough, PassThrough>>>&
        instances)
{
    add_device_operation_instances(instances,
                                   device_gemm_xdl_c_shuffle_i8_i8_i8_km_nk_mn_instances{});
}

} // namespace instance
} // namespace device
} // namespace tensor_operation
} // namespace ck
