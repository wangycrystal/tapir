#include "BeliefNode.hpp"

#include <ctime>                        // for clock, CLOCKS_PER_SEC, clock_t

#include <map>                          // for _Rb_tree_iterator, map<>::iterator, map
#include <memory>                       // for unique_ptr
#include <random>                       // for uniform_int_distribution
#include <set>
#include <tuple>                        // for tie, tuple
#include <utility>                      // for pair, make_pair, move

#include "global.hpp"                     // for RandomGenerator, make_unique

#include "geometry/Action.hpp"                   // for Action
#include "geometry/Observation.hpp"              // for Observation
#include "geometry/State.hpp"                    // for State

#include "mappings/ActionMapping.hpp"

#include "ActionNode.hpp"               // for ActionNode
#include "HistoryEntry.hpp"             // for HistoryEntry

#include "RandomAccessSet.hpp"

namespace solver {
long BeliefNode::currId = 0;
std::clock_t BeliefNode::startTime = std::clock();

BeliefNode::BeliefNode() :
    BeliefNode(nullptr, currId) {
}

BeliefNode::BeliefNode(std::unique_ptr<ActionMapping> actionMap) :
    BeliefNode(std::move(actionMap), currId) {
}

BeliefNode::BeliefNode(std::unique_ptr<ActionMapping> actionMap, long id) :
    id_(id),
    tLastAddedParticle_(0),
    tNNComp_(-1.0),
    nnBel_(nullptr),
    particles_(),
    actionMap_(std::move(actionMap)) {
    if (currId <= id) {
        currId = id + 1;
    }
}

// Do-nothing destructor
BeliefNode::~BeliefNode() {
}

bool BeliefNode::hasActionToTry() const {
    return actionMap_->hasActionToTry();
}

std::unique_ptr<Action> BeliefNode::getNextActionToTry() const {
    return actionMap_->getNextActionToTry();
}

std::unique_ptr<Action> BeliefNode::getSearchAction(double exploreCoefficient) {
    return actionMap_->getSearchAction(exploreCoefficient);
}

std::unique_ptr<Action> BeliefNode::getBestAction() const {
    return actionMap_->getBestAction();
}

double BeliefNode::getBestMeanQValue() const {
    return actionMap_->getBestMeanQValue();
}

void BeliefNode::updateBestValue() {
    actionMap_->updateBestValue();
}

void BeliefNode::addParticle(HistoryEntry *newHistEntry) {
    tLastAddedParticle_ = (double) (std::clock() - startTime)
        * 1000 / CLOCKS_PER_SEC;
    particles_.add(newHistEntry);
}

void BeliefNode::removeParticle(HistoryEntry *histEntry) {
    particles_.remove(histEntry);
}


HistoryEntry *BeliefNode::sampleAParticle(RandomGenerator *randGen) const {
    long index = std::uniform_int_distribution<long>(
                                 0, getNParticles() - 1)(*randGen);
    return particles_.get(index);
}

void BeliefNode::updateQValue(Action const &action, double increase) {
    updateQValue(action, increase, 0);
}

void BeliefNode::updateQValue(Action const &action, double increase,
        long deltaNParticles) {
    actionMap_->getActionNode(action)->updateQValue(increase, deltaNParticles);
    updateBestValue();
}

double BeliefNode::distL1Independent(BeliefNode *b) const {
    double dist = 0.0;
    for (HistoryEntry *entry1 : particles_) {
        for (HistoryEntry *entry2 : b->particles_) {
            dist += entry1->getState()->distanceTo(*entry2->getState());
        }
    }
    double averageDist = dist / (getNParticles() * b->getNParticles());
    if (averageDist < 0) {
        debug::show_message("ERROR: Distance < 0 between beliefs.");
    } else if (averageDist == 0) {
        debug::show_message("NOTE: Identical belief nodes found!");
    }
    return averageDist;
}

long BeliefNode::getId() const {
    return id_;
}

long BeliefNode::getNParticles() const {
    return particles_.size();
}

std::vector<State const *> BeliefNode::getStates() const {
    std::vector<State const *> states;
    for (HistoryEntry *entry : particles_) {
        states.push_back(entry->getState());
    }
    return states;
}

long BeliefNode::getNActChildren() const {
    return actionMap_->getNChildren();
}

ActionMapping *BeliefNode::getMapping() {
    return actionMap_.get();
}

BeliefNode *BeliefNode::getChild(Action const &action, Observation const &obs) const {
    ActionNode *node = actionMap_->getActionNode(action);
    if (node == nullptr) {
        return nullptr;
    }
    return node->getChild(obs);
}

std::pair<BeliefNode *, bool> BeliefNode::createOrGetChild(Action const &action,
        Observation const &obs) {
    ActionNode *actionNode = actionMap_->getActionNode(action);
    if (actionNode == nullptr) {
        actionNode = actionMap_->createActionNode(action);
    }
    return actionNode->createOrGetChild(obs);
}
} /* namespace solver */
