#include "TextToImageConverter.h"

int main()
{
    const char *outputImageFile = "imagem_saida.bmp";
    const char *outputTextFile = "mensagem_saida.txt";

    TextToImageConverter converter(nullptr, outputImageFile, nullptr, outputTextFile);

    if (converter.decodeTextFromImage())
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
