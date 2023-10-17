#include "TextToImageConverter.h"

int main()
{
    const char *inputImageFile = "imagem.bmp";
    const char *outputImageFile = "imagem_saida.bmp";
    const char *textFile = "mensagem.txt";

    TextToImageConverter converter(inputImageFile, outputImageFile, textFile, nullptr);

    if (converter.encodeTextInImage())
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
