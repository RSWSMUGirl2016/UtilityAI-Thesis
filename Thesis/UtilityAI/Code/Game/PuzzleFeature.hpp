#pragma once
#include "Game/Feature.hpp"

class PuzzleFeature : public Feature
{
public:
    int m_indexInSolvedOrder;

    PuzzleFeature();
    ~PuzzleFeature();

    void ChangeType(const std::string& puzzleFeatureName);
    virtual void Update();
    virtual void Activate();
};