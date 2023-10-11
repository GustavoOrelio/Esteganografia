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

    // Função para obter os bits menos significativos de um byte
    uint8_t getLSB(uint8_t byte, int bitCount)
    {
        return byte & ((1 << bitCount) - 1);
    }

    // Função para substituir os bits menos significativos de um byte
    uint8_t replaceLSB(uint8_t byte, uint8_t newBits, int bitCount)
    {
        return (byte & (0xFF << bitCount)) | newBits;
    }

    void modifyRGBChannels(BMP &image, int pixelIndex, char ch)
    {
        image(pixelIndex, 0)->Red = replaceLSB(image(pixelIndex, 0)->Red, (ch >> 5) & 7, 3);
        image(pixelIndex, 0)->Green = replaceLSB(image(pixelIndex, 0)->Green, (ch >> 2) & 7, 3);
        image(pixelIndex, 0)->Blue = replaceLSB(image(pixelIndex, 0)->Blue, ch & 3, 2);
    }

public:
    TextToImageConverter(const char *inputImageFile, const char *outputImageFile, const char *textFile, const char *outputTextFile)
        : inputImageFile(inputImageFile), outputImageFile(outputImageFile), textFile(textFile), outputTextFile(outputTextFile) {}

    // Função para codificar o texto na imagem
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
        int totalPixels = image.TellWidth() * image.TellHeight();

        // Codifica cada caractere do texto nos pixels da imagem
        while (textFileStream.get(ch))
        {
            if (pixelIndex >= totalPixels)
            {
                std::cerr << "Erro: A mensagem é muito longa para ser codificada na imagem." << std::endl;
                return false;
            }

            modifyRGBChannels(image, pixelIndex, ch);
            pixelIndex++;
        }

        // Marca o final da mensagem com um caractere nulo '\0'
        if (pixelIndex < totalPixels)
        {
            modifyRGBChannels(image, pixelIndex, '\0');
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

    // Função para decodificar o texto da imagem
    bool decodeTextFromImage()
    {
        BMP image;

        if (!image.ReadFromFile(outputImageFile))
        {
            std::cerr << "Erro ao abrir a imagem de saída." << std::endl;
            return false;
        }

        std::ofstream textFileStream(outputTextFile);
        if (!textFileStream)
        {
            std::cerr << "Erro ao criar o arquivo de texto." << std::endl;
            return false;
        }

        int totalPixels = image.TellWidth() * image.TellHeight();
        int pixelIndex = 0;

        std::ostringstream messageBuffer;
        // Decodifica a mensagem da imagem
        while (pixelIndex < totalPixels)
        {
            unsigned char ch = (getLSB(image(pixelIndex, 0)->Red, 3) << 5) | (getLSB(image(pixelIndex, 0)->Green, 3) << 2) | getLSB(image(pixelIndex, 0)->Blue, 2);
            // Verifica se é o final da mensagem com um caractere nulo '\0'
            if (ch == '\0')
                break;

            messageBuffer.put(ch);
            pixelIndex++;
        }

        textFileStream << messageBuffer.str();
        textFileStream.close();

        std::cout << "Mensagem recuperada e salva em '" << outputTextFile << "'." << std::endl;
        return true;
    }
};

int main()
{
    const char *inputImageFile = "imagem.bmp";
    const char *outputImageFile = "imagem_saida.bmp";
    const char *textFile = "mensagem.txt";
    const char *outputTextFile = "mensagem_saida.txt";

    TextToImageConverter converter(inputImageFile, outputImageFile, textFile, outputTextFile);

    if (converter.encodeTextInImage() && converter.decodeTextFromImage())
    {
        return 0;
    }
    else
    {
        return 1;
    }
}