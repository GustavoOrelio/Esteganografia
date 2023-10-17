#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include "EasyBMP.h"
#include <cstdint>

class TextToImageConverter
{
private:
    const char *inputImageFile;
    const char *outputImageFile;
    const char *textFile;
    const char *outputTextFile;

    uint8_t getLSB(uint8_t byte, int bitCount);
    uint8_t replaceLSB(uint8_t byte, uint8_t newBits, int bitCount);
    void modifyRGBChannels(BMP &image, int pixelIndex, char ch);

public:
    TextToImageConverter(const char *inputImageFile, const char *outputImageFile, const char *textFile, const char *outputTextFile);

    bool encodeTextInImage();
    bool decodeTextFromImage();
};
