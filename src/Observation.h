#ifndef OBSERVATION_H
#define OBSERVATION_H

#include <ostream>
#include <queue>

#include "Model.h"

class BeliefNode;

class Observation {
	public:
		Observation();
		Observation(ObsVals &o, BeliefNode* nxtBelNode);
		~Observation();

		bool isObs(ObsVals &o);
		BeliefNode* getNodeChild();
		void getChildren(std::queue<BeliefNode*> &res);
		void write(std::ostream &os);
		void writeNGetChildren(std::ostream &os, std::queue<BeliefNode*> &res);

	private:
		ObsVals vals;
		BeliefNode* child;

};
#endif
