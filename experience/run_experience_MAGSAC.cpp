//
// Created by clementriu on 10/6/20.
//

#include <string.h>
#include <fstream>
#include <vector>
#include <ctime>
#include <chrono>
#include <cstddef>
#include <mutex>
#include <memory>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/features2d.hpp>

#include "magsac_utils.h"
#include "utils.h"
#include "magsac.h"

#include "uniform_sampler.h"
#include "flann_neighborhood_graph.h"
#include "fundamental_estimator.h"
#include "homography_estimator.h"
#include "types.h"
#include "model.h"
#include "estimators.h"

//#include "experience_utils.hpp"
//#include "metrics.hpp"
#include "handle_exp.hpp"

int main(int argc, char **argv) {
    int iterMax = 10000;

    int modelUsed = 1; // 0 for Homography, 1 for Fundamental.

    double maxSigmaMagsac = 50; // TODO check if sigma or propTo sigma
    double magsacConfidence = 0.99; // The required confidence in the results
    double magsacRefThreshold = 2.0;

    bool useMagsacPP = true;

    int nGen = 1;
    int nRun = 1;

    bool verbose = true;
    unsigned int seed = (unsigned) time(0); // Use option -t for a reproducible run

    // TODO handle arguments

    std::srand(seed);


    std::vector<std::vector<int>> labelsVect;
    std::vector<std::vector<size_t>> possibleInliersVect;
    std::vector<std::vector<double>> weightsVect;
    std::vector<std::vector<int>> vecInliersVect;
    std::vector<std::vector<double>> errorsVect;

    const char *pathToInInliers = argv[1];
    const char *pathToInOutliers = argv[2];
    const char *pathToOutput = argv[3];
    const char *pathToOutLabels = argv[4];
    const char *pathToOutPossibleInliers = argv[5];
    const char *pathToOutWeights = argv[6];
    const char *pathToOutComputedInliers = argv[7];
    const char *pathToOutErrors = argv[8];

    cv::Mat points; // The point correspondences, each is of format "x1 y1 x2 y2"
    std::vector<int> groundTruthLabels; // The ground truth labeling provided in the dataset

    std::cout << "\nReading " << pathToInInliers << " and " << pathToInOutliers << std::endl;

    if (!ReadPoints(pathToInInliers, pathToInOutliers, points, groundTruthLabels)) {
        std::cerr << "Problem loading points !" << std::endl;
        return 1;
    }

    const size_t pointNumber = points.rows; // The number of points in the scene

    if (pointNumber == 0) // If there are no points, return
    {
        return 1; // TODO
    }

    labelsVect.push_back(groundTruthLabels);

    std::vector<double> precisionMagsacVect;
    std::vector<double> recallMagsacVect;
    std::vector<double> runtimeMagsacVect;

    for (int run = 0; run < nRun; run++) {


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
                     pointNumber,
                     magsacConfidence,
                     points,
                     useMagsacPP,
                     maxSigmaMagsac,
                     iterMax,
                     magsacRefThreshold,
                     possibleInliersVect,
                     weightsVect,
                     vecInliersVect,
                     errorsVect,
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
                     pointNumber,
                     magsacConfidence,
                     points,
                     useMagsacPP,
                     maxSigmaMagsac,
                     iterMax,
                     magsacRefThreshold,
                     possibleInliersVect,
                     weightsVect,
                     vecInliersVect,
                     errorsVect,
                     runtimeMagsacVect,
                     precisionMagsacVect,
                     recallMagsacVect);
        }
    }

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

    saveVectOfVect(pathToOutLabels, labelsVect);
    saveVectOfVect(pathToOutWeights, weightsVect);
    saveVectOfVect(pathToOutPossibleInliers, possibleInliersVect);
    saveVectOfVect(pathToOutComputedInliers, vecInliersVect);
    saveVectOfVect(pathToOutErrors, errorsVect);

    return 0;
}