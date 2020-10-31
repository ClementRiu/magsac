//
// Created by clementriu on 10/6/20.
//

#include "experience_utils.hpp"

bool ReadPoints(const char *fileInliers, const char *fileOutliers,
                cv::Mat &points, std::vector<int> &groundTruthLabels,
                const bool readOutliers) {
    std::vector<Match> ptsInliers;
    std::vector<Match> ptsOutliers;

    if (!Match::loadMatch(fileInliers, ptsInliers)) {
        std::cerr << "Problem loading inliers: " << fileInliers << std::endl;
        return false;
    }
    if (readOutliers) {
        if (!Match::loadMatch(fileOutliers, ptsOutliers)) {
            std::cerr << "Problem loading outliers: " << fileOutliers << std::endl;
            return false;
        }
    }
    std::vector<Match> ptsMixed;

    randomDataFusionWithMemory(ptsInliers, ptsOutliers, ptsMixed, 1, 0, groundTruthLabels);

    points.create(static_cast<int>(ptsMixed.size()), 4, CV_64F);
    for (int i = 0; i < ptsMixed.size(); ++i) {
        points.at<double>(i, 0) = ptsMixed[i].x1;
        points.at<double>(i, 1) = ptsMixed[i].y1;
        points.at<double>(i, 2) = ptsMixed[i].x2;
        points.at<double>(i, 3) = ptsMixed[i].y2;
    }
    return true;
}

bool
saveExpInfo(const char *nameFile, const unsigned int seed, const int nGen, const int nRun, const char *pathToIn1,
            const char *pathToIn2,
            const double AlgoPrecMean, const double AlgoPrecStd, const std::vector<double> &AlgoPrecVect,
            const double AlgoRecMean, const double AlgoRecStd, const std::vector<double> &AlgoRecVect,
            const double AlgoRuntimeMean, const double AlgoRuntimeStd, const std::vector<double> &AlgoRuntimeVect,
            const bool fullSave) {

    std::ofstream f(nameFile);

    if (f.is_open()) {
        f << "Input folder read: " << pathToIn1 << " & " << pathToIn2 << "\n";
        f << "Datasets generated with seed: " << seed << "\n";
        f << "\nResults over " << nGen << " random datasets and " << nRun << " runs.\n";

        f << "RANSAC: p= " << AlgoPrecMean << " | " << AlgoPrecStd
          << " r= " << AlgoRecMean << " | " << AlgoRecStd
          << " t= " << AlgoRuntimeMean << " | " << AlgoRuntimeStd << "\n";

        if (fullSave) {
            f << "\n";
            f << "RANSAC:\n\tp:\n";
            std::vector<double>::const_iterator AlgoPrecIt = AlgoPrecVect.begin();
            for (; AlgoPrecIt != AlgoPrecVect.end(); ++AlgoPrecIt) {
                f << *AlgoPrecIt << " ";
            }
            f << "\n\tr:\n";
            std::vector<double>::const_iterator AlgoRecIt = AlgoRecVect.begin();
            for (; AlgoRecIt != AlgoRecVect.end(); ++AlgoRecIt) {
                f << *AlgoRecIt << " ";
            }
            f << "\n\tt:\n";
            std::vector<double>::const_iterator AlgoRuntimeIt = AlgoRuntimeVect.begin();
            for (; AlgoRuntimeIt != AlgoRuntimeVect.end(); ++AlgoRuntimeIt) {
                f << *AlgoRuntimeIt << " ";
            }
            f << "\n";
        }
    }
    return f.is_open();
}