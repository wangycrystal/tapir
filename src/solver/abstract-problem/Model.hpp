#ifndef SOLVER_MODEL_HPP_
#define SOLVER_MODEL_HPP_

#include <memory>                       // for unique_ptr
#include <ostream>                      // for ostream
#include <vector>                       // for vector

#include "global.hpp"                     // for RandomGenerator

#include "solver/abstract-problem/Action.hpp"        // for Action
#include "solver/abstract-problem/ModelChange.hpp"                    // for State
#include "solver/abstract-problem/State.hpp"                    // for State
#include "solver/abstract-problem/Observation.hpp"              // for Observation
#include "solver/abstract-problem/Options.hpp"              // for Options
#include "solver/abstract-problem/TransitionParameters.hpp"

#include "solver/abstract-problem/heuristics/Heuristic.hpp"

namespace solver {
class ActionPool;
class BeliefNode;
class EstimationStrategy;
class HistoricalData;
class HistoryCorrector;
class ObservationPool;
class SearchStrategy;
class Serializer;
class Solver;
class StateIndex;
class StatePool;

class Model {
public:
    Model() = default;
    virtual ~Model() = default;
    _NO_COPY_OR_MOVE(Model);

    /** Returns the random number generator used by this model. */
    virtual RandomGenerator *getRandomGenerator() = 0;

    /** Returns the configuration options for this model. */
    virtual Options const *getOptions() = 0;

    /* --------------- The model interface proper ----------------- */
    /** Samples an initial state from the belief vector. */
    virtual std::unique_ptr<State> sampleAnInitState() = 0;
    /** Samples a state uniformly at random from all states. */
    virtual std::unique_ptr<State> sampleStateUniform() = 0;
    /** Returns true iff the given state is terminal. */
    virtual bool isTerminal(State const &state) = 0;


    /* -------------------- Black box dynamics ---------------------- */
    /** Represents the results of a complete step in the model,
     * including the next state, observation, and reward.
     */
    struct StepResult {
        std::unique_ptr<Action> action = nullptr;
        std::unique_ptr<TransitionParameters> transitionParameters = nullptr;
        std::unique_ptr<Observation> observation = nullptr;
        double reward = 0;
        std::unique_ptr<State> nextState = nullptr;
        bool isTerminal = false;
    };
    /** Generates the next state, an observation, and the reward. */
    virtual StepResult generateStep(State const &state, Action const &action) = 0;

    /** Generates the parameters for a next-state transition, if any are being
     * used - the default implementation simply returns nullptr.
     */
    virtual std::unique_ptr<TransitionParameters> generateTransition(State const &state,
            Action const &action);

    /** Generates the next state, based on the state and action, and,
     * if used, the transition parameters.
     */
    virtual std::unique_ptr<State> generateNextState(State const &state, Action const &action,
            TransitionParameters const *transitionParameters // optional
            ) = 0;

    /** Generates an observation, given the action and resulting next state;
     * optionally, the previous state and the transition parameters can also be
     * used.
     */
    virtual std::unique_ptr<Observation> generateObservation(State const *state, // optional
            Action const &action, TransitionParameters const *transitionParameters, // optional
            State const &nextState) = 0;

    /** Returns the reward for the given state, action; optionally this also
     * includes transition parameters and the next state - if they aren't
     * being used it is OK to use nullptr for those inputs.
     */
    virtual double generateReward(State const &state, Action const &action,
            TransitionParameters const *transitionParameters, // optional
            State const *nextState) = 0; // optional


    /* -------------- Methods for handling model changes ---------------- */
    virtual void applyChanges(std::vector<std::unique_ptr<ModelChange>> const &changes,
            Solver *solver);


    /* ------------ Methods for handling particle depletion -------------- */
    /** Generates new state particles based on the state particles of the
     * previous node, as well as on the action and observation.
     */
    virtual std::vector<std::unique_ptr<State>> generateParticles(BeliefNode *previousBelief,
            Action const &action, Observation const &obs, long nParticles,
            std::vector<State const *> const &previousParticles);
    /** Generates new state particles based only on the previous action and
     * observation, assuming a poorly-informed prior over previous states.
     *
     * This should only be used if the previous belief turns out to be
     * incompatible with the current observation.
     */
    virtual std::vector<std::unique_ptr<State>> generateParticles(BeliefNode *previousBelief,
            Action const &action, Observation const &obs, long nParticles);


    /* ------------------- Pretty printing methods --------------------- */
    /** Draws the environment map onto the given output stream. */
    virtual void drawEnv(std::ostream &/*os*/);
    /** Draws the current belief and/or the current state in the context of the
     * overall map onto the given output stream.
     */
    virtual void drawSimulationState(BeliefNode const *belief, State const &/*state*/,
            std::ostream &/*os*/);


    /* ---------------------- Basic customizations  ---------------------- */
    /** Returns the function that approximates the value of a history entry based on the history
     * and/or an estimate using a single state. */
    virtual Heuristic getHeuristicFunction();

    /** Allows for a basic rollout strategy based on the history and/or the specific state. */
    virtual std::unique_ptr<Action> getRolloutAction(HistoricalData const *data,
            State const *state);


    /* ------- Customization of more complex solver functionality  --------- */
    // These are factory methods to allow the data structures used to
    // be managed in a customizable way.

    /** Creates a StateIndex, which manages searching for states that
     * have been used in a StatePool.
     */
    virtual std::unique_ptr<StateIndex> createStateIndex();

    /** Creates a HistoryCorrector; defaults to a general one, but can
     * be made problem-specific.
     */
    virtual std::unique_ptr<HistoryCorrector> createHistoryCorrector(Solver *solver);

    /** Creates an ActionPool, which manages actions and creates ActionMappings */
    virtual std::unique_ptr<ActionPool> createActionPool(Solver *solver) = 0;

    /** Creates an ObservationPool, which manages observations and creates ObservationMappings. */
    virtual std::unique_ptr<ObservationPool> createObservationPool(Solver *solver);

    /** Creates a search strategy for use by the given solver. */
    virtual std::unique_ptr<SearchStrategy> createSearchStrategy(Solver *solver);

    /** Creates a strategy for estimating the value of belief nodes, for backprop. */
    virtual std::unique_ptr<EstimationStrategy> createEstimationStrategy(Solver *solver);

    /** Creates the historical data for the root node. */
    virtual std::unique_ptr<HistoricalData> createRootHistoricalData();

    /** Creates a serializer for the given solver. */
    virtual std::unique_ptr<Serializer> createSerializer(Solver *solver);
};
} /* namespace solver */

#endif /* SOLVER_MODEL_HPP_ */
