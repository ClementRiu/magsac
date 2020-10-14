//
// Created by clementriu on 10/6/20.
//

#ifndef MAGSAC_EXPERIENCE_UTILS_HPP
#define MAGSAC_EXPERIENCE_UTILS_HPP


#include <fstream>
#include <iostream>
#include <vector>

#include <opencv2/core.hpp>

/// Save matching position between two points.
struct Match {
    Match() {}

    Match(float ix1, float iy1, float ix2, float iy2)
            : x1(ix1), y1(iy1), x2(ix2), y2(iy2) {}

    float x1, y1, x2, y2;

//    /**
//    * Transform into matrix where each column is (x1 y1 x2 y2)^T.
//    * \param m The matches that we transform.
//    * \return Matrix 4xn where n is the size of vector \a m.
//    */
//    static libNumerics::matrix<double> toMat(const std::vector<Match> &m) {
//        libNumerics::matrix<double> M(4, static_cast<int>(m.size()));
//        std::vector<Match>::const_iterator it = m.begin();
//        for (int j = 0; it != m.end(); ++it, ++j) {
//            M(0, j) = it->x1;
//            M(1, j) = it->y1;
//            M(2, j) = it->x2;
//            M(3, j) = it->y2;
//        }
//        return M;
//    }

    /**
    * Load the corresponding matches from file.
    * \param nameFile   The file where matches were saved.
    * \param vec_match  The loaded corresponding points.
    * \return bool      True if everything was ok, otherwise false.
    */
    static bool loadMatch(const char *nameFile, std::vector<Match> &vec_match) {
        vec_match.clear();
        std::ifstream f(nameFile);
        while (f.good()) {
            std::string str;
            std::getline(f, str);
            if (f.good()) {
                std::istringstream s(str);
                Match m;
                s >> m;
                if (!s.fail())
                    vec_match.push_back(m);
            }
        }
        return !vec_match.empty();
    }

    friend std::ostream &operator<<(std::ostream &os, const Match &m) {
        return os << m.x1 << " " << m.y1 << " "
                  << m.x2 << " " << m.y2 << std::endl;
    }

    friend std::istream &operator>>(std::istream &in, Match &m) {
        return in >> m.x1 >> m.y1 >> m.x2 >> m.y2;
    }
};

bool
ReadPoints(const char *fileInliers, const char *fileOutliers, cv::Mat &points, std::vector<int> &groundTruthLabels);

template<typename T, typename U>
void
randomDataFusionWithMemory(const std::vector<T> &vect0, const std::vector<T> &vect1, std::vector<T> &mixedVector,
                           const U label0, const U label1, std::vector<U> &labels) {
    typename std::vector<T>::const_iterator it0 = vect0.begin();
    typename std::vector<T>::const_iterator it1 = vect1.begin();
    size_t size0 = vect0.size();
    size_t size1 = vect1.size();
    while (mixedVector.size() < size0 + size1) {
        if (it0 != vect0.end() && it1 != vect1.end()) {
            int chosenDataSet = std::rand() % 2;
            if (chosenDataSet == 0) {
                mixedVector.push_back(*it0);
                labels.push_back(label0);
                it0++;
            } else {
                mixedVector.push_back(*it1);
                labels.push_back(label1);
                it1++;
            }
        } else {
            if (it0 == vect0.end() && it1 != vect1.end()) {
                mixedVector.push_back(*it1);
                labels.push_back(label1);
                it1++;
            } else {
                if (it1 == vect1.end() && it0 != vect0.end()) {
                    mixedVector.push_back(*it0);
                    labels.push_back(label0);
                    it0++;
                } else {
                    std::cerr << "PROBLEM during fusion of dataset." << std::endl;
                    assert(false); // TODO: Raise exception rather than assert error.
                }
            }
        }
    }

}

template<typename T>
void printVector(const std::vector<T> vectToPrint) {
    typename std::vector<T>::const_iterator itVectToPrint = vectToPrint.begin();
    for (; itVectToPrint != vectToPrint.end(); ++itVectToPrint) {
        std::cout << *itVectToPrint << " ";
    }
    std::cout << std::endl;
}

bool
saveExpInfo(const char *nameFile, unsigned int seed, int nGen, int nRun, const char *pathToIn1, const char *pathToIn2,
            double AlgoPrecMean, double AlgoPrecStd, const std::vector<double> &AlgoPrecVect,
            double AlgoRecMean, double AlgoRecStd, const std::vector<double> &AlgoRecVect,
            double AlgoRuntimeMean, double AlgoRuntimeStd, const std::vector<double> &AlgoRuntimeVect,
            bool fullSave = false);

template<typename T>
bool saveVectOfVect(const char *nameFile, const std::vector<std::vector<T>> &vectOfVect) {
    std::ofstream f(nameFile);
    if (f.is_open()) {
        typename std::vector<std::vector<T>>::const_iterator itVectOfVect = vectOfVect.begin();
        for (; itVectOfVect != vectOfVect.end(); ++itVectOfVect) {
            typename std::vector<T>::const_iterator itVect = (*itVectOfVect).begin();
            for (; itVect != (*itVectOfVect).end(); ++itVect) {
                f << *itVect << ", ";
            }
            f << "\n";
        }
    }
    return f.is_open();
}

#endif //MAGSAC_EXPERIENCE_UTILS_HPP
