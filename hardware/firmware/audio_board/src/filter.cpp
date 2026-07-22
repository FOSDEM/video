#include "filter.h"

#include "debug.h"

bool _compare_band(const Band& a, const Band& b)
{
    return a.gain < b.gain;
}

void Filter::Apply()
{
    // Sort the filters in gain order to prevent clipping between filter stages
    Band sb[4];
    std::copy_n(&_band[0], 4, &sb[0]);
    std::sort(&sb[0], &sb[4], _compare_band);

    for (int i = 0; i < 4; i++)
    {
        this->biquad->setCoefficients(i, sb[i].coef);
    }
    this->dirty = true;
}
