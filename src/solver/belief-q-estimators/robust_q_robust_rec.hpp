#ifndef SOLVER_ROBUST_Q_ROBUST_REC_HPP_
#define SOLVER_ROBUST_Q_ROBUST_REC_HPP_

#include "solver/abstract-problem/Action.hpp"

#include "solver/mappings/actions/ActionMapping.hpp"

#include "estimation.hpp"

namespace solver {
class RobustQRobustChildStrategy: public BeliefEstimationStrategy {
    virtual std::unique_ptr<BeliefQValueEstimator> createEstimator(ActionMapping *mapping);
};

class RobustQRobustChild: public BeliefQValueEstimator {
public:
    RobustQRobustChild(ActionMapping *mapping);
    virtual ~RobustQRobustChild() = default;
    _NO_COPY_OR_MOVE(RobustQRobustChild);

    /** Recalculates the q-value as the average q-value of its action children (weighted by
     *  the visit counts), and sets the recommended action as the action with the highest q-value.
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

#endif /* SOLVER_ROBUST_Q_ROBUST_REC_HPP_ */
