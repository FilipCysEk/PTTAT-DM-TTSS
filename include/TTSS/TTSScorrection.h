//
// Created by krzysiu on 11.07.2020.
//

#ifndef TRAMHOLDUP_TTSSCORRECTION_H
#define TRAMHOLDUP_TTSSCORRECTION_H

namespace TTSSdata {
    static const int locationCorrection = 3600000;

    static double locationCorrect(int number) {
        return (double) number / locationCorrection;
    }
}

#endif //TRAMHOLDUP_TTSSCORRECTION_H
