#include <iostream>
#include <fstream>
#include "EasyBMP.h"
#include <cstdint>

class TextToImageConverter
{
public:
    TextToImageConverter(const char *inputImageFile, const char *outputImageFile, const char *textFile, const char *outputTextFile)
        : inputImageFile(inputImageFile), outputImageFile(outputImageFile), textFile(textFile), outputTextFile(outputTextFile) {}

    bool encodeTextInImage()
    {
        BMP image;
        if (!image.ReadFromFile(inputImageFile))
        {
            std::cerr << "Erro ao abrir a imagem de entrada." << std::endl;
            return false;
        }

        std::ifstream textFileStream(textFile);
        if (!textFileStream)
        {
            std::cerr << "Erro ao abrir o arquivo de texto." << std::endl;
            return false;
        }

        char ch;
        int pixelIndex = 0;
        while (textFileStream.get(ch))
        {
            // 3 bits no canal vermelho
            image(pixelIndex, 0)->Red = replaceLSB(image(pixelIndex, 0)->Red, (ch >> 5) & 7, 3);
            // 3 bits no canal verde
            image(pixelIndex, 0)->Green = replaceLSB(image(pixelIndex, 0)->Green, (ch >> 2) & 7, 3);
            // 2 bits no canal azul
            image(pixelIndex, 0)->Blue = replaceLSB(image(pixelIndex, 0)->Blue, ch & 3, 2);
            pixelIndex++;
        }

        textFileStream.close();

        if (!image.WriteToFile(outputImageFile))
        {
            std::cerr << "Erro ao salvar a imagem de saída." << std::endl;
            return false;
        }

        std::cout << "Mensagem codificada com sucesso na imagem." << std::endl;
        return true;
    }

    bool decodeTextFromImage()
    {
        BMP image;
        if (!image.ReadFromFile(outputImageFile))
        {
            std::cerr << "Erro ao abrir a imagem de saida." << std::endl;
            return false;
        }

        std::ofstream textFileStream(outputTextFile);
        if (!textFileStream)
        {
            std::cerr << "Erro ao criar o arquivo de texto." << std::endl;
            return false;
        }

        for (int i = 0; i < image.TellWidth(); i++)
        {
            char ch = 0;
            ch |= (image(i, 0)->Red & 7) << 5;
            ch |= (image(i, 0)->Green & 7) << 2;
            ch |= image(i, 0)->Blue & 3;
            if (ch == '\0')
                break;
            textFileStream.put(ch);
        }

        textFileStream.close();

        std::cout << "Mensagem recuperada e salva em '" << outputTextFile << "'." << std::endl;
        return true;
    }

private:
    const char *inputImageFile;
    const char *outputImageFile;
    const char *textFile;
    const char *outputTextFile;

    // Função para substituir os bits menos significativos de um byte com novos bits
    uint8_t replaceLSB(uint8_t byte, int newBits, int bitCount)
    {
        return (byte & (0xFF << bitCount)) | newBits;
    }
};

int main()
{
    const char *inputImageFile = "imagem.bmp";
    const char *outputImageFile = "imagem_saida.bmp";
    const char *textFile = "mensagem.txt";
    const char *outputTextFile = "mensagem_saida.txt";

    TextToImageConverter converter(inputImageFile, outputImageFile, textFile, outputTextFile);

    // Codificar a mensagem na imagem
    if (converter.encodeTextInImage())
    {
        // Decodificar a mensagem da imagem
        if (converter.decodeTextFromImage())
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        return 1;
    }
}
