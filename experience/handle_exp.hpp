//
// Created by clementriu on 10/7/20.
//

#include <opencv2/core.hpp>
#include <vector>

#include "magsac_utils.h"
#include "magsac.h"

#include "uniform_sampler.h"

#include "experience_utils.hpp"
#include "metrics.hpp"


template<typename T, typename U>
bool runAnExp(T &estimator,
              U &model,
              const std::vector<int> &groundTruthLabels,
              const bool verbose,
              const double magsacConfidence,
              const cv::Mat &points,
              const bool useMagsacPP,
              const size_t partitionNumber,
              const double inverseMaxTime,
              const double maxSigmaMagsac,
              const int iterMax,
              const double magsacRefThreshold,
              gcransac::sampler::UniformSampler &mainSampler,
              std::vector<std::vector<size_t>> &possibleInliersVect,
              std::vector<std::vector<double>> &weightsVect,
              std::vector<std::vector<int>> &vecInliersVect,
              std::vector<std::vector<double>> &errorsVect,
              std::vector<std::vector<double>> &errorsAllVect,
              std::vector<double> &runtimeMagsacVect,
              std::vector<double> &precisionMagsacVect,
              std::vector<double> &recallMagsacVect,
              const double normalizing_multiplier = 1) {


    std::vector<int> groundTruthInliers = getSubsetFromLabeling(groundTruthLabels, 1);

    // Initialize the sampler used for selecting minimal samples

    typename MAGSAC<cv::Mat, T>::Version magsacVersion;
    if (useMagsacPP) {
        magsacVersion = MAGSAC<cv::Mat, T>::Version::MAGSAC_PLUS_PLUS;
    } else {
        magsacVersion = MAGSAC<cv::Mat, T>::Version::MAGSAC_ORIGINAL;
    }

    MAGSAC<cv::Mat, T> magsac(magsacVersion);
    magsac.setMaximumThreshold(maxSigmaMagsac); // The maximum noise scale sigma allowed
    magsac.setIterationLimit(iterMax); // Iteration limit to interrupt the cases when the algorithm run too long.
    magsac.setReferenceThreshold(magsacRefThreshold);
    magsac.setPartitionNumber(partitionNumber);
    magsac.setFPS(inverseMaxTime);

    int iterationNumber = 0; // Number of iterations required
    ModelScore score; // The model score
    std::vector<size_t> indexSavedInliers;
    std::vector<double> weightsSavedInliers;
    std::vector<double> errorsSavedInliers;

    std::vector<int> vec_inliersMagsac;
    bool okMagsac = false;

    double errorMagsac = 0;

    int TMagsac;
    double runtimeMagsac;


    clock_t begin = std::clock();
    okMagsac = magsac.run(points, // The data points
                          magsacConfidence, // The required confidence in the results
                          estimator, // The used estimator
                          mainSampler, // The sampler used for selecting minimal samples in each iteration
                          model, // The estimated model
                          iterationNumber, // The number of iterations
                          score,
                          indexSavedInliers,
                          weightsSavedInliers); // The score of the estimated model
    clock_t end = std::clock();
    runtimeMagsac = (double) (end - begin) / CLOCKS_PER_SEC;

    possibleInliersVect.push_back(indexSavedInliers);
    weightsVect.push_back(weightsSavedInliers);

    cv::Mat inlierPoints;
    transformToInliers(indexSavedInliers, weightsSavedInliers, points, inlierPoints, vec_inliersMagsac); // TODO

    if (okMagsac) {
        computeModelError(vec_inliersMagsac, points, estimator, model, errorsSavedInliers);
        errorMagsac = meanOfVect(errorsSavedInliers);
    }

    vecInliersVect.push_back(vec_inliersMagsac);
    errorsVect.push_back(errorsSavedInliers);

    std::vector<double> errorsAll;
    computeModelError(points, estimator, model, errorsAll, normalizing_multiplier);
    errorsAllVect.push_back(errorsAll);

    int numTruePositivesMagsac = 0;
    double precisionMagsac = 0;
    double recallMagsac = 0;
    int inlierCountMagsac = inlierPoints.rows;

    if (okMagsac) {
        runtimeMagsacVect.push_back(runtimeMagsac);

        numTruePositivesMagsac = computeTruePositive(vec_inliersMagsac, groundTruthLabels);
        precisionMagsac = computePrecision(numTruePositivesMagsac, vec_inliersMagsac.size());
        recallMagsac = computeRecall(numTruePositivesMagsac, groundTruthInliers.size());
    }

    precisionMagsacVect.push_back(precisionMagsac);
    recallMagsacVect.push_back(recallMagsac);

    if (okMagsac && verbose) {
        std::cout << "\nMagsac" << std::endl;
        std::cout << "\tModel=" << " ["
                  << model.descriptor(0, 0) << ", "
                  << model.descriptor(0, 1) << ", "
                  << model.descriptor(0, 2) << ", "
                  << model.descriptor(1, 0) << ", "
                  << model.descriptor(1, 1) << ", "
                  << model.descriptor(1, 2) << ", "
                  << model.descriptor(2, 0) << ", "
                  << model.descriptor(2, 1) << ", "
                  << model.descriptor(2, 2) << " ]"
                  << std::endl;

        std::cout << "\tInlier error: " << errorMagsac
                  << "\n\tNumber of inliers: " << inlierCountMagsac
                  << "\n\tNumber of iterations: " << iterationNumber << "\n\tVPM: NOT IMPLEMENTED"
                  << "\n\tRuntime: " << runtimeMagsac
                  << std::endl;
        std::cout << "\tNumber true positives: " << numTruePositivesMagsac
                  << "\n\tPrecision: " << precisionMagsac
                  << "\n\tRecall: " << recallMagsac
                  << std::endl;

        printf("\n\tActual number of iterations drawn by MAGSAC at %.2f confidence: %d\n", magsacConfidence,
               iterationNumber);
    }

    return okMagsac;
}