#include "BeliefNode.hpp"

#include <map>                          // for _Rb_tree_iterator, map<>::iterator, map
#include <memory>                       // for unique_ptr
#include <random>                       // for uniform_int_distribution
#include <set>
#include <tuple>                        // for tie, tuple
#include <utility>                      // for pair, make_pair, move

#include "global.hpp"                     // for RandomGenerator, make_unique
#include "RandomAccessSet.hpp"

#include "ActionNode.hpp"               // for ActionNode
#include "HistoryEntry.hpp"             // for HistoryEntry
#include "Solver.hpp"                   // for Solver

#include "abstract-problem/Action.hpp"                   // for Action
#include "abstract-problem/Observation.hpp"              // for Observation
#include "abstract-problem/State.hpp"                    // for State

#include "mappings/actions/ActionMapping.hpp"
#include "mappings/observations/ObservationMapping.hpp"
#include "mappings/observations/ObservationPool.hpp"

#include "search/HistoricalData.hpp"

namespace solver {
BeliefNode::BeliefNode() :
            BeliefNode(-1, nullptr) {
}
BeliefNode::BeliefNode(ObservationMappingEntry *parentEntry) :
            BeliefNode(-1, parentEntry) {
}
BeliefNode::BeliefNode(long id, ObservationMappingEntry *parentEntry) :
            id_(id),
            depth_(-1),
            parentEntry_(parentEntry),
            data_(nullptr),
            particles_(),
            nStartingSequences_(0),
            tLastChange_(-1),
            actionMap_(nullptr),
            estimator_(nullptr) {
}

// Do-nothing destructor
BeliefNode::~BeliefNode() {
}

/* ----------------- Useful calculations ------------------- */
double BeliefNode::distL1Independent(BeliefNode *b) const {
    double dist = 0.0;
    for (HistoryEntry *entry1 : particles_) {
        for (HistoryEntry *entry2 : b->particles_) {
            dist += entry1->getState()->distanceTo(*entry2->getState());
        }
    }
    double averageDist = dist / (getNumberOfParticles() * b->getNumberOfParticles());
    if (averageDist < 0) {
        debug::show_message("ERROR: Distance < 0 between beliefs.");
    } else if (averageDist == 0) {
        debug::show_message("NOTE: Identical belief nodes found!");
    }
    return averageDist;
}

/* -------------------- Simple getters ---------------------- */
long BeliefNode::getId() const {
    return id_;
}
long BeliefNode::getDepth() const {
    return depth_;
}
long BeliefNode::getNumberOfParticles() const {
    return particles_.size();
}
long BeliefNode::getNumberOfStartingSequences() const {
    return nStartingSequences_;
}
std::vector<State const *> BeliefNode::getStates() const {
    std::vector<State const *> states;
    for (HistoryEntry *entry : particles_) {
        states.push_back(entry->getState());
    }
    return states;
}
double BeliefNode::getTimeOfLastChange() const {
    return tLastChange_;
}

/* -------------------- Tree-related getters  ---------------------- */
ActionMapping *BeliefNode::getMapping() const {
    return actionMap_.get();
}
ObservationMappingEntry *BeliefNode::getParentEntry() const {
    return parentEntry_;
}
ActionNode *BeliefNode::getParentActionNode() const {
    if (parentEntry_ == nullptr) {
        return nullptr;
    }
    return parentEntry_->getMapping()->getOwner();
}
HistoricalData *BeliefNode::getHistoricalData() const {
    return data_.get();
}
BeliefNode *BeliefNode::getParentBelief() const {
    if (parentEntry_ == nullptr) {
        return nullptr;
    }
    return getParentActionNode()->getParentEntry()->getMapping()->getOwner();
}
std::unique_ptr<Observation> BeliefNode::getLastObservation() const {
    if (parentEntry_ == nullptr) {
        return nullptr;
    }
    return parentEntry_->getObservation();
}
std::unique_ptr<Action> BeliefNode::getLastAction() const {
    if (parentEntry_ == nullptr) {
        return nullptr;
    }
    return getParentActionNode()->getParentEntry()->getAction();
}
BeliefNode *BeliefNode::getChild(Action const &action, Observation const &obs) const {
    ActionNode *node = actionMap_->getActionNode(action);
    if (node == nullptr) {
        return nullptr;
    }
    return node->getChild(obs);
}

/* -------------- Wrappers for estimator methods ---------------- */
std::unique_ptr<Action> BeliefNode::getRecommendedAction() const {
    return estimator_->getRecommendedAction();
}
double BeliefNode::getQValue() const {
    return estimator_->getBeliefQValue();
}
void BeliefNode::recalculate() {
    estimator_->recalculate();
}

/* ============================ PRIVATE ============================ */

/* -------------- Particle management / sampling ---------------- */
void BeliefNode::addParticle(HistoryEntry *newHistEntry) {
    tLastChange_ = abt::clock_ms();
    particles_.add(newHistEntry);
    if (newHistEntry->getId() == 0) {
        nStartingSequences_++;
    }
}

void BeliefNode::removeParticle(HistoryEntry *histEntry) {
    tLastChange_ = abt::clock_ms();
    particles_.remove(histEntry);
    if (histEntry->getId() == 0) {
        nStartingSequences_--;
    }
}

/* -------------------- Tree-related setters  ---------------------- */
void BeliefNode::setMapping(std::unique_ptr<ActionMapping> mapping) {
    actionMap_ = std::move(mapping);
    actionMap_->setOwner(this);
}
void BeliefNode::setEstimator(std::unique_ptr<BeliefQValueEstimator> estimator) {
    estimator_ = std::move(estimator);
}
void BeliefNode::setHistoricalData(std::unique_ptr<HistoricalData> data) {
    data_ = std::move(data);
}

/* -------------------- Tree-related methods  ---------------------- */
std::pair<BeliefNode *, bool> BeliefNode::createOrGetChild(Solver *solver, Action const &action,
        Observation const &obs) {
    ActionNode *actionNode = actionMap_->getActionNode(action);
    if (actionNode == nullptr) {
        actionNode = actionMap_->createActionNode(action);
        actionNode->setMapping(solver->getObservationPool()->createObservationMapping());
    }
    return actionNode->createOrGetChild(solver, obs);
}
} /* namespace solver */
