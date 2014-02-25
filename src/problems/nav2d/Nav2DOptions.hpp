#ifndef NAV2DOPTIONS_HPP_
#define NAV2DOPTIONS_HPP_

#include <sstream>                      // for basic_stringbuf<>::int_type, basic_stringbuf<>::pos_type, basic_stringbuf<>::__streambuf_type
#include <string>                       // for string

#include <boost/program_options.hpp>    // for value, options_description, options_description_easy_init, typed_value

#include "problems/shared/ProgramOptions.hpp"  // for ProgramOptions

namespace po = boost::program_options;

namespace nav2d {
class Nav2DOptions : public ProgramOptions {
    /** Returns configurations options for the Nav2D POMDP */
    po::options_description getProblemOptions() {
        po::options_description problem("Settings specific to the Nav2D POMDP");
        problem.add(ProgramOptions::getProblemOptions());
        problem.add_options()
                ("problem.mapPath,m", po::value<std::string>(),
                        "path to map file")
                ("problem.timeStepLength,m", po::value<double>(),
                        "path to map file")
                ("problem.costPerUnitDistance", po::value<double>(),
                        "cost per unit distance travelled"),
                ("problem.costPerRevolution", po::value<double>(),
                        "cost per 360 degrees of turning"),
                ("problem.speedErrorType", po::value<std::string>(),
                        "type of error in the speed signal")
                ("problem.speedErrorSD", po::value<double>(),
                        "standard deviation for errors in the speed")
                ("problem.rotationErrorType", po::value<std::string>(),
                        "standard deviation for errors in the speed")
                ("problem.rotationErrorSD", po::value<double>(),
                        "standard deviation for errors in angular velocity;"
                        "measured in turns (360deg) per unit time.");
        return problem;
    }

    /** Returns configuration options for the Nav2D heuristic */
    po::options_description getHeuristicOptions() {
        po::options_description heuristic("Nav2D heuristic configuration");
        heuristic.add(ProgramOptions::getHeuristicOptions());
        return heuristic;
    }
};
} /* namespace nav2d */

#endif /* NAV2DOPTIONS_HPP_ */