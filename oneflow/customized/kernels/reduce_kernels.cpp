#include "oneflow/core/framework/framework.h"
#include "oneflow/core/ndarray/ndarray_util.h"
#include "oneflow/core/ndarray/xpu_var_ndarray.h"

namespace oneflow {

namespace {

template<template<typename> class BinaryFunc, DeviceType device_type, typename T>
class ReduceKernel final : public user_op::OpKernel {
 public:
  ReduceKernel() = default;
  ~ReduceKernel() = default;

 private:
  void Compute(user_op::KernelComputeContext* ctx) const override {
    const user_op::Tensor* input_tensor = ctx->Tensor4ArgNameAndIndex("input_tensor", 0);
    user_op::Tensor* output_tensor = ctx->Tensor4ArgNameAndIndex("output_tensor", 0);
    user_op::Tensor* tmp_buffer = ctx->Tensor4ArgNameAndIndex("tmp_buffer", 0);
    const auto& reduce_axes_vec = ctx->Attr<AxisVector>("reduce_axes");
    const Shape& reduced_shape = CreateReducedShape(input_tensor->shape(), reduce_axes_vec);
    NdarrayReduce<device_type, T, BinaryFunc>::Reduce(
        ctx->device_ctx(), XpuVarNdarray<T>(reduced_shape, output_tensor->mut_dptr<T>()),
        XpuVarNdarray<const T>(input_tensor->shape(), input_tensor->dptr<T>()),
        XpuVarNdarray<T>(tmp_buffer->shape(), tmp_buffer->mut_dptr<T>()));
  }
  bool AlwaysComputeWhenAllOutputsEmpty() const override { return false; }
};

template<DeviceType device_type, typename T>
bool IsMatchedPred(const user_op::KernelRegContext& ctx) {
  const user_op::TensorDesc* output_tensor_desc =
      ctx.TensorDesc4ArgNameAndIndex("output_tensor", 0);
  return ctx.device_type() == device_type
         && output_tensor_desc->data_type() == GetDataType<T>::value;
}

template<typename T>
size_t InferTmpSize(user_op::InferContext* ctx) {
  const Shape* in_shape = ctx->Shape4ArgNameAndIndex("input_tensor", 0);
  return in_shape->elem_cnt() * sizeof(T);
}

}  // namespace

#define REGISTER_REDUCE_XPU_KERNEL(op_name, binary_func, device, dtype) \
  REGISTER_USER_KERNEL(op_name)                                         \
      .SetCreateFn<ReduceKernel<binary_func, device, dtype>>()          \
      .SetIsMatchedPred(IsMatchedPred<device, dtype>)                   \
      .SetInferTmpSizeFn(InferTmpSize<dtype>);

#define REGISTER_REDUCE_BY_DEVICETYPE(device, dtype)                       \
  REGISTER_REDUCE_XPU_KERNEL("reduce_prod", BinaryFuncProd, device, dtype) \
  REGISTER_REDUCE_XPU_KERNEL("reduce_min", BinaryFuncMin, device, dtype)   \
  REGISTER_REDUCE_XPU_KERNEL("reduce_any", BinaryFuncAny, device, dtype)   \
  REGISTER_REDUCE_XPU_KERNEL("reduce_all", BinaryFuncAll, device, dtype)   \
  REGISTER_REDUCE_XPU_KERNEL("reduce_sum", BinaryFuncSum, device, dtype)

#define REGISTER_REDUCE_KERNEL(dtype)                    \
  REGISTER_REDUCE_BY_DEVICETYPE(DeviceType::kCPU, dtype) \
  REGISTER_REDUCE_BY_DEVICETYPE(DeviceType::kGPU, dtype)

REGISTER_REDUCE_KERNEL(float)
REGISTER_REDUCE_KERNEL(double)
REGISTER_REDUCE_KERNEL(int32_t)
REGISTER_REDUCE_KERNEL(int64_t)
REGISTER_REDUCE_XPU_KERNEL("reduce_sum", BinaryFuncSum, DeviceType::kGPU, float16)

}  // namespace oneflow