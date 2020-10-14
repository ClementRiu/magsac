//
// Created by clementriu on 10/7/20.
//

#ifndef MAGSAC_METRICS_HPP
#define MAGSAC_METRICS_HPP

#include <vector>

#include <opencv2/core.hpp>

void transformToInliers(const std::vector<size_t> &inliersIdxsSaved, const std::vector<double> &weightsSaved,
                        const cv::Mat &allPoints,
                        cv::Mat &inlierPoints, std::vector<int> &vec_inliersMagsac);

/// Compute number of true positives from a list of estimated positive indexes and a label vector of 1 for relevant elements and 0 for irrelevant elements.
/// \param[in] positiveIdx: Vector of indexes of the elements estimated positives.
/// \param[in] labels: Vector of labels of all data.
/// Must be 1 for a relevant element and 0 for an irrelevant one.
/// \return The number of true positives.
int computeTruePositive(const std::vector<int> &positiveIdx, const std::vector<int> &labels);

/// Compute the precision: tp / (tp + fp)
/// \param[in] numTruePositive: Number of true positives.
/// \param[in] numEstimatedPositive: Number of estimated positives.
/// \return The precision.
double computePrecision(int numTruePositive, int numEstimatedPositive);

/// Compute the recall. tp / (tp + fn)
/// \param[in] numTruePositive: Number of true positives.
/// \param[in] numEstimatedPositive: Number of true inliers.
/// \return The recall.
double computeRecall(int numTruePositive, int numInlier);

//// Compute mean of a vect.
//// \param vect [in] vector on which to compute the mean.
template<typename T>
double meanOfVect(const std::vector<T> vect) {
    double mean = 0;
    typename std::vector<T>::const_iterator it = vect.begin();
    for (; it != vect.end(); it++) {
        mean += *it;
    }
    return mean / vect.size();
}

//// Compute STD of a vect if mean is given.
//// \param mean [in] mean of the vect.
//// \param vect [in] vector on which to compute the std.
template<typename T>
double standardDeviation(const double mean, const std::vector<T> vect) {
    double var = 0;
    typename std::vector<T>::const_iterator it = vect.begin();
    for (; it != vect.end(); it++) {
        var += (*it - mean) * (*it - mean);
    }
    return std::sqrt(var / vect.size());
}

template<typename T, typename U>
double
computePointError(const int indexToCompute, const cv::Mat &points, const T &estimator, const U &model) {
    cv::Mat pointToCompute;
    pointToCompute.create(1, 4, CV_64F);
    pointToCompute.at<double>(0, 0) = points.at<double>(indexToCompute, 0);
    pointToCompute.at<double>(0, 1) = points.at<double>(indexToCompute, 1);
    pointToCompute.at<double>(0, 2) = points.at<double>(indexToCompute, 2);
    pointToCompute.at<double>(0, 3) = points.at<double>(indexToCompute, 3);

    return estimator.residual(pointToCompute, model);
}

template<typename T, typename U>
void
computeModelError(const std::vector<int> &indexesToCompute, const cv::Mat &points, const T &estimator, const U &model,
                  std::vector<double> &errors) {
    std::vector<int>::const_iterator itIndexesToCompute = indexesToCompute.begin();
    for (; itIndexesToCompute != indexesToCompute.end(); itIndexesToCompute++){
        errors.push_back(computePointError(*itIndexesToCompute, points, estimator, model));
    }
}


#endif //MAGSAC_METRICS_HPP
