#include <iostream>
#include <fstream>
#include <sstream>
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
        int totalPixels = image.TellWidth() * image.TellHeight();

        while (textFileStream.get(ch))
        {
            if (pixelIndex >= totalPixels || pixelIndex < 0)
            {
                std::cerr << "Erro: Índice de pixel fora dos limites." << std::endl;
            }
            else
            {
                // Extração dos bits da mensagem para o canal vermelho (3 bits)
                unsigned char redBits = (ch >> 5) & 7;

                // Substituição dos 3 bits menos significativos do canal vermelho
                image(pixelIndex, 0)->Red = replaceLSB(image(pixelIndex, 0)->Red, redBits, 3);

                // Extração dos bits da mensagem para o canal verde (3 bits)
                unsigned char greenBits = (ch >> 2) & 7; 

                // Substituição dos 3 bits menos significativos do canal verde
                image(pixelIndex, 0)->Green = replaceLSB(image(pixelIndex, 0)->Green, greenBits, 3);

                // Extração dos bits da mensagem para o canal azul (2 bits)
                unsigned char blueBits = ch & 3; 

                // Substituição dos 2 bits menos significativos do canal azul
                image(pixelIndex, 0)->Blue = replaceLSB(image(pixelIndex, 0)->Blue, blueBits, 2);
            }

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

        int totalPixels = image.TellWidth() * image.TellHeight(); 
        int pixelIndex = 0;
        int x = pixelIndex % image.TellWidth();
        int y = pixelIndex / image.TellWidth();
        std::ostringstream messageBuffer;
        while (pixelIndex < totalPixels)
        {
            unsigned char redBits = getLSB(image(x, y)->Red, 3);
            unsigned char greenBits = getLSB(image(x, y)->Green, 3);
            unsigned char blueBits = getLSB(image(x, y)->Blue, 2);
            unsigned char ch = (redBits << 5) | (greenBits << 2) | blueBits;
            messageBuffer.put(ch);
            pixelIndex++;
        }

        std::string mensagem = messageBuffer.str();
        textFileStream << mensagem; 

        textFileStream.close();

        std::cout << "Mensagem recuperada e salva em '" << outputTextFile << "'." << std::endl;
        return true;
    }

private:
    const char *inputImageFile;
    const char *outputImageFile;
    const char *textFile;
    const char *outputTextFile;

    // Função getLSB adicionada para extrair os bits menos significativos
    uint8_t getLSB(uint8_t byte, int bitCount)
    {
        return byte & ((1 << bitCount) - 1);
    }

    // Função para substituir os bits menos significativos de um byte com novos bits
    uint8_t replaceLSB(uint8_t byte, uint8_t newBits, int bitCount)
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
