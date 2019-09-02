#include "oneflow/core/common/error_util.h"

namespace oneflow {

std::shared_ptr<ErrorProto> ErrorUtil::Ok() { return std::make_shared<ErrorProto>(); }

std::shared_ptr<ErrorProto> ErrorUtil::ProtoParseFailedError(const std::string& msg) {
  auto error = std::make_shared<ErrorProto>();
  error->set_msg(msg);
  error->mutable_proto_parse_failed_error();
  return error;
}

std::shared_ptr<ErrorProto> ErrorUtil::JobSetEmpty(const std::string& msg) {
  auto error = std::make_shared<ErrorProto>();
  error->set_msg(msg);
  error->set_job_build_and_infer_error(JobBuildAndInferError::kJobSetEmpty);
  return error;
}

std::shared_ptr<ErrorProto> ErrorUtil::DeviceTagNotFound(const std::string& msg) {
  auto error = std::make_shared<ErrorProto>();
  error->set_msg(msg);
  error->set_job_build_and_infer_error(JobBuildAndInferError::kDeviceTagNotFound);
  return error;
}

std::shared_ptr<ErrorProto> ErrorUtil::JobTypeNotSet(const std::string& msg) {
  auto error = std::make_shared<ErrorProto>();
  error->set_msg(msg);
  error->set_job_build_and_infer_error(JobBuildAndInferError::kJobTypeNotSet);
  return error;
}

}  // namespace oneflow
