#ifndef NAV2D_ACTION_HPP_
#define NAV2D_ACTION_HPP_

#include <cstddef>                      // for size_t

#include <ostream>                      // for ostream
#include <vector>                       // for vector

#include "solver/geometry/Action.hpp"
#include "solver/geometry/EnumeratedPoint.hpp"

namespace nav2d {
class Nav2DAction : public solver::EnumeratedPoint {
    friend class Nav2DTextSerializer;
  public:
    Nav2DAction(long code, double speed, double rotationalSpeed);

    virtual ~Nav2DAction() = default;
    // Copy constructor is allowed, but not others.
    Nav2DAction(Nav2DAction const &other);
    Nav2DAction(Nav2DAction &&) = delete;
    Nav2DAction &operator=(Nav2DAction const &) = delete;
    Nav2DAction &operator=(Nav2DAction &&) = delete;

    std::unique_ptr<solver::Action> copy() const override;
    double distanceTo(solver::Action const &otherAction) const override;
    bool equals(solver::Action const &otherAction) const override;
    std::size_t hash() const override;
    void print(std::ostream &os) const override;

    double getSpeed() const;
    double getRotationalSpeed() const;

    long getCode() const;
    void calculateCode();

  private:
    long code_;
    double speed_;
    double rotationalSpeed_;
};
} /* namespace nav2d */

#endif /* NAV2D_ACTION_HPP_ */
