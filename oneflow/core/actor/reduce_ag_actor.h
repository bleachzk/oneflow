#ifndef ONEFLOW_CORE_ACTOR_REDUCE_AG_ACTOR_H_
#define ONEFLOW_CORE_ACTOR_REDUCE_AG_ACTOR_H_

#include "oneflow/core/actor/actor.h"

namespace oneflow {

class ReduceAGActor final : public Actor {
 public:
  OF_DISALLOW_COPY_AND_MOVE(ReduceAGActor);
  ReduceAGActor() = default;
  ~ReduceAGActor() = default;

  void VirtualActorInit(const TaskProto&) override;

 private:
  void Act(std::function<bool(Regst*)>* IsNaiveAllowedReturnToProducer) override;
  bool IsCustomizedReadReady() override;
  void NormalProcessCustomizedReadableRegstMsg(const ActorMsg&) override;
  std::pair<bool, std::vector<std::string>> GetNaiveConsumedRegstDescName() override {
    return {false, {}};
  }

  int64_t processed_regst_cnt_;
  HashMap<int64_t, std::queue<Regst*>> in_regsts_;
  HashMap<int64_t, Regst*> regsts_in_using_;
};

}  // namespace oneflow

#endif  // ONEFLOW_CORE_ACTOR_REDUCE_AG_ACTOR_H_
