#include "Nav2DTextSerializer.hpp"

#include <iostream>                     // for operator<<, basic_ostream, basic_istream<>::__istream_type, basic_ostream<>::__ostream_type, cerr, endl
#include <string>                       // for operator>>, string
#include <vector>                       // for vector

#include "global.hpp"                     // for make_unique
#include "problems/shared/GridPosition.hpp"  // for GridPosition

#include "solver/geometry/Action.hpp"
#include "solver/geometry/State.hpp"
#include "solver/geometry/Observation.hpp"

#include "solver/mappings/enumerated_actions.hpp"
#include "solver/mappings/enumerated_observations.hpp"

#include "solver/serialization/TextSerializer.hpp"    // for TextSerializer

#include "Nav2DAction.hpp"         // for Nav2DAction
#include "Nav2DObservation.hpp"    // for Nav2DObservation
#include "Nav2DState.hpp"          // for Nav2DState

namespace solver {
class Solver;
} /* namespace solver */

namespace nav2d {
Nav2DTextSerializer::Nav2DTextSerializer(solver::Solver *solver) :
    solver::Serializer(solver) {
}

void Nav2DTextSerializer::saveState(solver::State const *state,
        std::ostream &os) {
    if (state == nullptr) {
        os << "()";
        return;
    }
    Nav2DState const &navState =
        static_cast<Nav2DState const &>(*state);
    os << "(" << navState.x_ << " " << navState.y_;
    os << "):" << navState.direction_;
}

std::unique_ptr<solver::State> Nav2DTextSerializer::loadState(
        std::istream &is) {
    std::string tmpStr;
    std::getline(is, tmpStr, '(');
    std::getline(is, tmpStr, ')');
    if (tmpStr == "") {
        return nullptr;
    }
    double x, y, direction;
    std::istringstream iss(tmpStr);
    iss >> x;
    iss >> y;
    std::getline(is, tmpStr, ':');
    is >> direction;
    return std::make_unique<Nav2DState>(x, y, direction);
}

void Nav2DTextSerializer::saveObservation(solver::Observation const *obs,
        std::ostream &os) {
    if (obs == nullptr) {
        os << "()";
        return;
    }
    Nav2DObservation const &observation =
            static_cast<Nav2DObservation const &>(*obs);
    if (observation.isEmpty()) {
        os << "(NONE)";
    } else {
        saveState(observation.state_.get(), os);
    }
}

std::unique_ptr<solver::Observation> Nav2DTextSerializer::loadObservation(
        std::istream &is) {
    std::string tmpStr;
    std::getline(is, tmpStr, '(');
    std::getline(is, tmpStr, ')');
    if (tmpStr == "") {
        return nullptr;
    } else if (tmpStr == "NONE") {
        return std::make_unique<Nav2DObservation>();
    }
    double x, y, direction;
    std::istringstream iss(tmpStr);
    iss >> x;
    iss >> y;
    std::getline(is, tmpStr, ':');
    is >> direction;
    return std::make_unique<Nav2DObservation>(x, y, direction);
}


void Nav2DTextSerializer::saveAction(solver::Action const *action,
        std::ostream &os) {
    if (action == nullptr) {
        os << "NULL";
        return;
    }
    Nav2DAction const &a =
            static_cast<Nav2DObservation const &>(*action);
    os << a.speed_ << "/" << a.rotationalSpeed_;
}

std::unique_ptr<solver::Action> Nav2DTextSerializer::loadAction(
        std::istream &is) {
    std::string text;
    is >> text;
    if (text == "NULL") {
        return nullptr;
    }
    std::istringstream iss(text);
    double speed, rotationalSpeed;
    std::getline()
    Nav2DAction const &a =
            static_cast<Nav2DObservation const &>(*action);
    os << a.speed_ << "/" << a.rotationalSpeed_;
}

} /* namespace nav2d */