//
// Created by clementriu on 10/7/20.
//

#include "metrics.hpp"


void transformToInliers(const std::vector<size_t> &inliersIdxsSaved, const std::vector<double> &weightsSaved,
                        const cv::Mat &allPoints,
                        cv::Mat &inlierPoints, std::vector<int> &vec_inliersMagsac) {

    int iter = 0;
    size_t numInliers = inliersIdxsSaved.size();
    inlierPoints.create(static_cast<int>(numInliers), 4, CV_64F);

    std::vector<size_t>::const_iterator itInliersIdxSaved = inliersIdxsSaved.begin();
    for (; itInliersIdxSaved != inliersIdxsSaved.end(); itInliersIdxSaved++) {
        inlierPoints.at<double>(iter, 0) = allPoints.at<double>(*itInliersIdxSaved, 0);
        inlierPoints.at<double>(iter, 1) = allPoints.at<double>(*itInliersIdxSaved, 1);
        inlierPoints.at<double>(iter, 2) = allPoints.at<double>(*itInliersIdxSaved, 2);
        inlierPoints.at<double>(iter, 3) = allPoints.at<double>(*itInliersIdxSaved, 3);
        iter++;

        vec_inliersMagsac.push_back(static_cast<int>(*itInliersIdxSaved));
    }
}

/// Compute number of true positives from a list of estimated positive indexes and a label vector of 1 for relevant elements and 0 for irrelevant elements.
/// \param[in] positiveIdx: Vector of indexes of the elements estimated positives.
/// \param[in] labels: Vector of labels of all data.
/// Must be 1 for a relevant element and 0 for an irrelevant one.
/// \return The number of true positives.
int computeTruePositive(const std::vector<int> &estimatedPositiveIdx, const std::vector<int> &labels) {
    int numTruePositives = 0;
    std::vector<int>::const_iterator itEstimatedPositiveIdx = estimatedPositiveIdx.begin();
    for (; itEstimatedPositiveIdx != estimatedPositiveIdx.end(); itEstimatedPositiveIdx++) {
        numTruePositives += labels[*itEstimatedPositiveIdx];
    }
    return numTruePositives;
}

/// Compute the precision: tp / (tp + fp)
/// \param[in] numTruePositive: Number of true positives.
/// \param[in] numEstimatedPositive: Number of estimated positives.
/// \return The precision.
double computePrecision(const int numTruePositive, const int numEstimatedPositive) {
    return numTruePositive / (double) numEstimatedPositive;
}

/// Compute the recall. tp / (tp + fn)
/// \param[in] numTruePositive: Number of true positives.
/// \param[in] numEstimatedPositive: Number of true inliers.
/// \return The recall.
double computeRecall(const int numTruePositive, const int numInlier) {
    return numTruePositive / (double) numInlier;
}