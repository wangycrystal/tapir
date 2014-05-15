#ifndef SOLVER_MAX_Q_MAX_REC_HPP_
#define SOLVER_MAX_Q_MAX_REC_HPP_

#include "solver/abstract-problem/Action.hpp"

#include "solver/mappings/actions/ActionMapping.hpp"

#include "estimation.hpp"

namespace solver {
class MaxQMaxChildStrategy: public BeliefEstimationStrategy {
    virtual std::unique_ptr<BeliefQValueEstimator> createEstimator(ActionMapping *mapping);
};

class MaxQMaxChild: public BeliefQValueEstimator {
public:
    MaxQMaxChild(ActionMapping *mapping);
    virtual ~MaxQMaxChild() = default;
    _NO_COPY_OR_MOVE(MaxQMaxChild);

    /** Recalculates the q-value as the maximum q-value of its action children,
     * and sets the recommended action as the action with this q-value.
     */
    virtual void recalculate() override;

    /** Return the average q-value for the belief, at the time of last recalculation. */
    virtual double getBeliefQValue() const override;

    /** Return the action with the highest q-value at the time of last recalculation,
     * or a random unvisited action if no actions have been visited. */
    virtual std::unique_ptr<Action> getRecommendedAction() const override;

private:
    ActionMapping *mapping_;

    double beliefQValue_;
    std::unique_ptr<Action> recommendedAction_;
};
} /* namespace solver */

#endif /* SOLVER_MAX_Q_MAX_REC_HPP_ */
