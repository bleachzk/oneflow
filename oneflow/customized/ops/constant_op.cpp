#include "oneflow/core/common/shape_vec.h"
#include "oneflow/core/framework/framework.h"
#include "oneflow/core/framework/sbp_context.h"
#include "oneflow/core/framework/user_op_attr.pb.h"

namespace oneflow {
REGISTER_USER_OP("constant")
    .Output("out")
    .Attr("floating_value", UserOpAttrType::kAtDouble)
    .Attr("integer_value", UserOpAttrType::kAtInt64)
    .Attr("is_floating_value", UserOpAttrType::kAtBool)
    .Attr("dtype", UserOpAttrType::kAtDataType)
    .Attr("shape", UserOpAttrType::kAtShape)
    .SetTensorDescInferFn([](user_op::InferContext* ctx) -> Maybe<void> {
      Shape* out_shape = ctx->Shape4ArgNameAndIndex("out", 0);
      const Shape& shape = ctx->GetAttr<Shape>("shape");
      auto dtype = ctx->GetAttr<DataType>("dtype");
      DimVector dim_vec;
      if (shape.NumAxes() > 0) {
        dim_vec.insert(dim_vec.end(), shape.dim_vec().cbegin(), shape.dim_vec().cend());
      }
      if (dim_vec.empty()) { dim_vec.push_back(1); }
      *ctx->Dtype4ArgNameAndIndex("out", 0) = dtype;
      *out_shape = Shape(dim_vec);
      return Maybe<void>::Ok();
    })
    .SetBatchAxisInferFn([](user_op::BatchAxisContext* ctx) -> Maybe<void> {
      ctx->BatchAxis4ArgNameAndIndex("out", 0)->clear_value();
      return Maybe<void>::Ok();
    })
    .SetGetSbpFn([](user_op::SbpContext* ctx) -> Maybe<void> {
      SbpSignatureBuilder()
          .Broadcast(ctx->inputs())
          .Broadcast(ctx->outputs())
          .Build(ctx->sbp_sig_list()->mutable_sbp_signature()->Add());
      return Maybe<void>::Ok();
    });
}  // namespace oneflow