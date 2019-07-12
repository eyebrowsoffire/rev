#include "rev/CurveFile.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

namespace rev {
namespace {
    bool shouldSkipLine(const std::string& line)
    {
        for (char c : line) {
            if (std::isspace(c)) {
                continue;
            }
            if (c == '#') {
                return true;
            }
            return false;
        }
        return true;
    }

    std::string getNextLine(std::ifstream& file)
    {
        for (;;) {
            if (file.eof()) {
                return "";
            }

            std::string line;
            if (!std::getline(file, line)) {
                return "";
            }

            if (!shouldSkipLine(line)) {
                return line;
            }
        }
    }

    NurbsCurve<glm::vec3> parseFile(const std::string& filePath)
    {
        std::ifstream file{ filePath };
        if (!file) {
            throw std::runtime_error("Unable to read curve file.");
        }

        size_t curveOrder;
        std::string line = getNextLine(file);
        if (line.empty()) {
            throw std::runtime_error("No curve order found in curve file.");
        }

        std::istringstream lineStream(line);
        std::string lineType;
        lineStream >> lineType;
        if (lineType != "o") {
            throw std::runtime_error("No curve order found in curve file.");
        }

        lineStream >> curveOrder;

        line = getNextLine(file);
        if (line.empty()) {
            throw std::runtime_error("No control points found in curve file.");
        }

        if (line != "cp") {
            throw std::runtime_error("No control points found in curve file.");
        }

        std::vector<WeightedControlPoint<glm::vec3>> controlPoints;
        for (;;) {
            line = getNextLine(file);
            if (line.empty()) {
                throw std::runtime_error("No knots found in curve file.");
            }

            if (line == "k") {
                break;
            }

            std::istringstream lineStream(line);
            auto& controlPoint = controlPoints.emplace_back();

            lineStream >> controlPoint.point.x;
            lineStream >> controlPoint.point.y;
            lineStream >> controlPoint.point.z;
            lineStream >> controlPoint.weight;
        }

        std::vector<float> knots;
        for (;;) {
            line = getNextLine(file);
            if (line.empty()) {
                break;
            }
            knots.push_back(std::stof(line));
        }

        return NurbsCurve<glm::vec3>{ curveOrder, knots, controlPoints };
    }
}

CurveFile::CurveFile(const std::string& filePath)
    : _curve(parseFile(filePath))
{
}

const NurbsCurve<glm::vec3>& CurveFile::getCurve() { return _curve; }
}
