#include "Model.hpp"

#include <functional>

#include "solver/cached_values.hpp"
#include "solver/ActionNode.hpp"
#include "solver/BeliefNode.hpp"


#include "solver/abstract-problem/Action.hpp"        // for Action
#include "solver/abstract-problem/HistoricalData.hpp"
#include "solver/abstract-problem/State.hpp"                    // for State
#include "solver/abstract-problem/Observation.hpp"              // for Observation
#include "solver/abstract-problem/TransitionParameters.hpp"
#include "solver/abstract-problem/heuristics/heuristics.hpp"

#include "solver/belief-estimators/estimators.hpp"

#include "solver/mappings/actions/ActionMapping.hpp"
#include "solver/mappings/actions/ActionPool.hpp"
#include "solver/mappings/observations/discrete_observations.hpp"
#include "solver/mappings/observations/ObservationMapping.hpp"
#include "solver/mappings/observations/ObservationPool.hpp"

#include "solver/indexing/StateIndex.hpp"
#include "solver/indexing/RTree.hpp"

#include "solver/changes/DefaultHistoryCorrector.hpp"
#include "solver/changes/HistoryCorrector.hpp"

#include "solver/search/search_interface.hpp"
#include "solver/search/steppers/ucb_search.hpp"

#include "solver/serialization/Serializer.hpp"

namespace solver {
/* ----------------------- Basic getters  ----------------------- */
std::string Model::getName() {
    return "Default Model";
}

/* ---------- Virtual getters for ABT / model parameters  ---------- */
bool Model::hasColorOutput() {
    return false;
}
bool Model::hasVerboseOutput() {
    return false;
}

/* -------------------- Black box dynamics ---------------------- */
// Transition parameters are optional
std::unique_ptr<TransitionParameters> Model::generateTransition(
        State const &/*state*/,
        Action const &/*action*/) {
    return nullptr;
}


/* -------------- Methods for handling model changes ---------------- */
// Default = no changes.
void Model::applyChange(ModelChange const &/*change*/, StatePool */*pool*/) {
}


/* ------------ Methods for handling particle depletion -------------- */
std::vector<std::unique_ptr<State>> Model::generateParticles(
        BeliefNode *previousBelief,
        Action const &action, Observation const &obs,
        long nParticles,
        std::vector<State const *> const &previousParticles) {
    std::vector<std::unique_ptr<State>> particles;
    ObservationMapping *obsMap = previousBelief->getMapping()->getActionNode(
            action)->getMapping();
    BeliefNode *childNode = obsMap->getBelief(obs);
    while ((long)particles.size() < nParticles) {
        long index = std::uniform_int_distribution<long>(0,
                previousParticles.size() - 1)(*getRandomGenerator());
        State const *state = previousParticles[index];
        StepResult result = generateStep(*state, action);
        if (obsMap->getBelief(*result.observation) == childNode) {
            particles.push_back(std::move(result.nextState));
        }
    }
    return particles;
}

std::vector<std::unique_ptr<State>> Model::generateParticles(
        BeliefNode *previousBelief,
        Action const &action, Observation const &obs,
        long nParticles) {
    std::vector<std::unique_ptr<State>> particles;
    ObservationMapping *obsMap = previousBelief->getMapping()->getActionNode(
            action)->getMapping();
    BeliefNode *childNode = obsMap->getBelief(obs);
    while ((long)particles.size() < nParticles) {
        std::unique_ptr<State> state = sampleStateUniform();
        StepResult result = generateStep(*state, action);
        if (obsMap->getBelief(*result.observation) == childNode) {
            particles.push_back(std::move(result.nextState));
        }
    }
    return particles;
}


/* --------------- Pretty printing methods ----------------- */
// Default = do nothing.
void Model::drawEnv(std::ostream &/*os*/) {
}
void Model::drawSimulationState(BeliefNode const */*belief*/, State const &/*state*/,
        std::ostream &/*os*/) {
}

/* ---------------------- Basic customizations  ---------------------- */
double Model::getHeuristicValue(HistoricalData const */*data*/, State const */*state*/) {
    return 0;
}

std::unique_ptr<Action> Model::getRolloutAction(HistoricalData const */*data*/,
        State const */*state*/) {
    return nullptr;
}

/* ------- Customization of more complex solver functionality  --------- */
std::unique_ptr<StateIndex> Model::createStateIndex() {
    return std::make_unique<RTree>(getNumberOfStateVariables());
}

std::unique_ptr<HistoryCorrector> Model::createHistoryCorrector(Solver *solver) {
    return std::make_unique<DefaultHistoryCorrector>(solver,
            heuristics::get_default_heuristic(this));
}

std::unique_ptr<ObservationPool> Model::createObservationPool(Solver */*solver*/) {
    return std::make_unique<DiscreteObservationPool>();
}

std::unique_ptr<SearchStrategy> Model::createSearchStrategy(Solver *solver) {
    return std::make_unique<BasicSearchStrategy>(solver,
            std::make_unique<UcbStepGeneratorFactory>(solver, 1.0),
            heuristics::get_default_heuristic(this));
}

std::unique_ptr<EstimationStrategy> Model::createEstimationStrategy(Solver */*solver*/) {
    return std::make_unique<EstimationFunction>(estimators::average_q_value);
}

std::unique_ptr<HistoricalData> Model::createRootHistoricalData() {
    return nullptr;
}

std::unique_ptr<Serializer> Model::createSerializer(Solver */*solver*/) {
    return nullptr;
}
} /* namespace solver */
