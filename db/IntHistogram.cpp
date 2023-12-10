#include <db/IntHistogram.h>
#include <sstream>
#include <vector>

using namespace db;

IntHistogram::IntHistogram(int buckets, int min, int max)
        : numBuckets(buckets), minVal(min), maxVal(max), totalValues(0) {
    // TODO pa4.1: some code goes here

    if (numBuckets <= 0) {
        throw std::invalid_argument("Number of buckets must be positive");
    }
    if (minVal > maxVal) {
        throw std::invalid_argument("Minimum value cannot be greater than maximum value");
    }
    bucketsVec=std::vector<int>(numBuckets, 0);
}

void IntHistogram::addValue(int v) {
    // TODO pa4.1: some code goes here
    int bucketIndex = (v - minVal) * numBuckets / (maxVal - minVal + 1);
    bucketIndex = std::max(0, std::min(bucketIndex, numBuckets - 1));
    bucketsVec[bucketIndex]++;
    totalValues++;
}

double IntHistogram::estimateSelectivity(Predicate::Op op, int v) const {
    // TODO pa4.1: some code goes here
    if (totalValues == 0) {
        return 0.0;
    }

    double selectivity = 0.0;

    // Handle cases where v is outside the histogram range
    if (v < minVal) {
        if (op == Predicate::Op::GREATER_THAN || op == Predicate::Op::GREATER_THAN_OR_EQ) {
            return 1.0;
        } else if (op == Predicate::Op::LESS_THAN || op == Predicate::Op::LESS_THAN_OR_EQ) {
            return 0.0;
        }
    } else if (v > maxVal) {
        if (op == Predicate::Op::LESS_THAN || op == Predicate::Op::LESS_THAN_OR_EQ) {
            return 1.0;
        } else if (op == Predicate::Op::GREATER_THAN || op == Predicate::Op::GREATER_THAN_OR_EQ) {
            return 0.0;
        }
    }

    int bucketIndex = std::min(std::max((v - minVal) * numBuckets / (maxVal - minVal + 1), 0), numBuckets - 1);
    int bucketWidth = (maxVal - minVal + 1) / numBuckets;
    int bucketStart = minVal + bucketIndex * bucketWidth;
    int bucketEnd = bucketStart + bucketWidth - 1;

    if (numBuckets <= 0) {
        throw std::runtime_error("numBuckets must be greater than zero");
    }
    // Ensure bucketWidth is never zero
    if (bucketWidth <= 0) {
        throw std::runtime_error("bucketWidth must be greater than zero");
    }

    switch (op) {
        case Predicate::Op::EQUALS: {
            if (v >= minVal && v <= maxVal) {
                int height = bucketsVec[bucketIndex];
                selectivity = static_cast<double>(height) / bucketWidth / totalValues;
            }
            break;
        }
        case Predicate::Op::GREATER_THAN: {
            for (int i = bucketIndex + 1; i < numBuckets; i++) {
                selectivity += static_cast<double>(bucketsVec[i]) / totalValues;
            }
            if (v >= bucketStart && v <= bucketEnd) {
                double bucketFraction = static_cast<double>(bucketEnd - v) / bucketWidth;
                selectivity += bucketFraction * bucketsVec[bucketIndex] / totalValues;
            }
            break;
        }
        case Predicate::Op::LESS_THAN: {
            for (int i = 0; i < bucketIndex; i++) {
                selectivity += static_cast<double>(bucketsVec[i]) / totalValues;
            }
            if (v >= bucketStart && v <= bucketEnd) {
                double bucketFraction = static_cast<double>(v - bucketStart) / bucketWidth;
                selectivity += bucketFraction * bucketsVec[bucketIndex] / totalValues;
            }
            break;
        }
        case Predicate::Op::GREATER_THAN_OR_EQ: {
            return estimateSelectivity(Predicate::Op::GREATER_THAN, v - 1);
        }
        case Predicate::Op::LESS_THAN_OR_EQ: {
            return estimateSelectivity(Predicate::Op::LESS_THAN, v + 1);
        }
        case Predicate::Op::NOT_EQUALS: {
            if (v < minVal || v > maxVal) {
                // If 'v' is outside the range, all values in the histogram are not equal to 'v'.
                selectivity = 1.0;
            } else {
                // Calculate the selectivity for equals and subtract from 1
                double equalsSelectivity = estimateSelectivity(Predicate::Op::EQUALS, v);
                selectivity = 1.0 - equalsSelectivity;
            }
            break;
        }

    }

    return selectivity;
}

double IntHistogram::avgSelectivity() const {
    // TODO pa4.1: some code goes here
    double totalSelectivity = 0.0;

    // Iterate over all buckets to calculate their individual selectivity
    for (int i = 0; i < numBuckets; i++) {
        double bucketSelectivity = static_cast<double>(bucketsVec[i]) / totalValues;
        totalSelectivity += bucketSelectivity;
    }

    // Calculate the average selectivity
    double avgSel = totalSelectivity / numBuckets;

    return avgSel;
}

std::string IntHistogram::to_string() const {
    // TODO pa4.1: some code goes here
    std::stringstream ss;
    ss << "IntHistogram(";
    ss << "numBuckets: " << numBuckets << ", ";
    ss << "minVal: " << minVal << ", ";
    ss << "maxVal: " << maxVal << ", ";
    ss << "buckets: [";
    for (const auto& count : bucketsVec) {
        ss << count << ", ";
    }
    // Remove the last comma and space if the buckets are not empty
    if (!bucketsVec.empty()) {
        ss.seekp(-2, ss.cur); // Move back two characters
        ss << "]"; // Overwrite the last ", " with "]"
    } else {
        ss << "]";
    }
    ss << ")";
    return ss.str();
}
