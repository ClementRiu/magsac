//
// Created by clementriu on 10/6/20.
//

#include <string.h>
#include <fstream>
#include <vector>
#include <ctime>
#include <mutex>
#include <opencv2/core.hpp>

#include "magsac.h"

#include "model.h"
#include "estimators.h"

#include "handle_exp.hpp"
#include "cmdLine.hpp"

int main(int argc, char **argv) {
    int iterMax = 10000;
    int modelUsed = 0;

    double maxSigmaMagsac = 50; // TODO check if sigma or propTo sigma
    double magsacConfidence = 0.99; // The required confidence in the results
    double magsacRefThreshold = 2.0;

    int nGen = 1;
    int nRun = 1;

    unsigned int seed = (unsigned) time(0); // Use option -t for a reproducible run

    utility::CmdLine cmd;
    cmd.add(utility::make_option('i', iterMax, "iterMax")
                    .doc("Number of iterations of Orsa and Ransac."));
    cmd.add(utility::make_option('u', modelUsed, "modelUsed")
                    .doc("Model used: 0 for homography, 1 for fundamental."));
    cmd.add(utility::make_switch('p', "magsacPlusPlus")
                    .doc("Add to use Magsac++"));

    cmd.add(utility::make_option('s', maxSigmaMagsac, "maxSigmaMagsac")
                    .doc("Maximum threshold for Magsac. To set high."));
    cmd.add(utility::make_option('c', magsacConfidence, "confidenceMagsac")
                    .doc("Confidence in the result."));
    cmd.add(utility::make_option('r', magsacRefThreshold, "magsacRefThreshold")
                    .doc("Reference threshold for magsac."));

    cmd.add(utility::make_switch('o', "ignoreOutliers")
                    .doc("Set to ignore outliers."));
    cmd.add(utility::make_option('e', nGen, "num-gen-exp")
                    .doc("Number of noisy datasets generated for the experience."));
    cmd.add(utility::make_option('n', nRun, "num-run")
                    .doc("Number of run of the algorithm."));

    cmd.add(utility::make_switch('v', "verbose")
                    .doc("Print info during the run."));
    cmd.add(utility::make_option('t', seed, "time-seed")
                    .doc("Use value instead of time for random seed (for debug)."));

    try {
        cmd.process(argc, argv);
    } catch (const std::string &s) {
        std::cerr << s << std::endl;
        return 1;
    }

    bool verbose = cmd.used('v');
    bool useMagsacPP = cmd.used('p');
    bool readOutliers = !cmd.used('o');

    std::srand(seed);

    std::vector<std::vector<size_t>> possibleInliersVect;
    std::vector<std::vector<double>> weightsVect;
    std::vector<std::vector<int>> vecInliersVect;
    std::vector<std::vector<double>> errorsVect;
    std::vector<std::vector<double>> errorsAllVect;

    std::vector<double> precisionMagsacVect;
    std::vector<double> recallMagsacVect;
    std::vector<double> runtimeMagsacVect;

    const char *pathToInInliers = argv[1];
    const char *pathToInOutliers = argv[2];
    const char *pathToOutput = argv[3];
    const char *pathToOutLabels = argv[4];
    const char *pathToOutPossibleInliers = argv[5];
    const char *pathToOutWeights = argv[6];
    const char *pathToOutComputedInliers = argv[7];
    const char *pathToOutErrors = argv[8];
    const char *pathToOutErrorsAll = argv[9];

    std::vector<cv::Mat> pointsAll; // The point correspondences, each is of format "x1 y1 x2 y2"
    std::vector<std::vector<int>> groundTruthLabelsAll; // The ground truth labeling provided in the dataset

    std::cout << "\nReading " << pathToInInliers << " and " << pathToInOutliers << std::endl;

    if (!ReadPoints(pathToInInliers, pathToInOutliers, nGen, pointsAll, groundTruthLabelsAll, readOutliers)) {
        std::cerr << "Problem loading points !" << std::endl;
        return 1;
    }

    for (int gen = 0; gen < nGen; gen++) {
        cv::Mat points = pointsAll[gen];
        std::vector<int> groundTruthLabels = groundTruthLabelsAll[gen];

        const size_t pointNumber = points.rows; // The number of points in the scene

        if (pointNumber == 0) // If there are no points, return
        {
            return 1; // TODO
        }
        gcransac::sampler::UniformSampler mainSampler(&points, true, seed);


        for (int run = 0; run < nRun; run++) {
            if (!verbose) {
                std::cout << "\rDataset " << gen + 1 << " out of " << nGen << " - Experiment " << run + 1 << " out of "
                          << nRun << std::flush;
            }

            if (modelUsed == 0) {
                if (verbose) {
                    printf("\tEstimated model = 'homography'.\n");
                }

                magsac::utils::DefaultHomographyEstimator estimator; // The robust homography estimator class containing the function for the fitting and residual calculation
                gcransac::Homography model; // The estimated model

                runAnExp(estimator,
                         model,
                         groundTruthLabels,
                         verbose,
                         magsacConfidence,
                         points,
                         useMagsacPP,
                         maxSigmaMagsac,
                         iterMax,
                         magsacRefThreshold,
                         mainSampler,
                         possibleInliersVect,
                         weightsVect,
                         vecInliersVect,
                         errorsVect,
                         errorsAllVect,
                         runtimeMagsacVect,
                         precisionMagsacVect,
                         recallMagsacVect);
            }
            if (modelUsed == 1) {
                if (verbose) {
                    printf("\tEstimated model = 'fundamental'.\n");
                }

                magsac::utils::DefaultFundamentalMatrixEstimator estimator(
                        maxSigmaMagsac); // The robust homography estimator class containing the function for the fitting and residual calculation
                gcransac::FundamentalMatrix model; // The estimated model

                runAnExp(estimator,
                         model,
                         groundTruthLabels,
                         verbose,
                         magsacConfidence,
                         points,
                         useMagsacPP,
                         maxSigmaMagsac,
                         iterMax,
                         magsacRefThreshold,
                         mainSampler,
                         possibleInliersVect,
                         weightsVect,
                         vecInliersVect,
                         errorsVect,
                         errorsAllVect,
                         runtimeMagsacVect,
                         precisionMagsacVect,
                         recallMagsacVect);
            }
        } // End of the run loop.
        if (!verbose) {
            std::cout << std::endl;
        }
    } // End of the gen loop.
    std::cout << std::endl;

    double precisionMagsacMean = meanOfVect(precisionMagsacVect);
    double recallMagsacMean = meanOfVect(recallMagsacVect);
    double precisionMagsacStd = standardDeviation(precisionMagsacMean, precisionMagsacVect);
    double recallMagsacStd = standardDeviation(recallMagsacMean, recallMagsacVect);
    double runtimeMagsacMean = meanOfVect(runtimeMagsacVect);
    double runtimeMagsacStd = standardDeviation(runtimeMagsacMean, runtimeMagsacVect);

    std::cout << "Over " << nGen << " datasets with " << nRun << " runs, for Magsac:\n"
              << "\tPrecision: " << precisionMagsacMean << " +/- " << precisionMagsacStd << "\n"
              << "\tRecall: " << recallMagsacMean << " +/- " << recallMagsacStd << "\n"
              << "\tRuntime: " << runtimeMagsacMean << " +/- " << runtimeMagsacStd << "\n"
              << std::endl;

    saveExpInfo(pathToOutput, seed, nGen, nRun, pathToInInliers, pathToInOutliers,
                precisionMagsacMean, precisionMagsacStd, precisionMagsacVect,
                recallMagsacMean, recallMagsacStd, recallMagsacVect,
                runtimeMagsacMean, runtimeMagsacStd, runtimeMagsacVect,
                true);

    saveVectOfVect(pathToOutLabels, groundTruthLabelsAll);
    saveVectOfVect(pathToOutWeights, weightsVect);
    saveVectOfVect(pathToOutPossibleInliers, possibleInliersVect);
    saveVectOfVect(pathToOutComputedInliers, vecInliersVect);
    saveVectOfVect(pathToOutErrors, errorsVect);
    saveVectOfVect(pathToOutErrorsAll, errorsAllVect);

    return 0;
}