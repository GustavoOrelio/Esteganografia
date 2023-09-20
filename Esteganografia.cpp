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
        for (int i = 0; i < image.TellWidth() && textFileStream.get(ch); ++i)
        {
            // Itera pelos pixels da imagem e substitui os bits menos significativos pelos bits do texto
            for (int j = 0; j < 3; ++j)
            { // R, G, B
                if (i < image.TellWidth())
                {
                    image(i, 0)->Red = replaceLSB(image(i, 0)->Red, ch);
                    image(i, 0)->Green = replaceLSB(image(i, 0)->Green, ch);
                    image(i, 0)->Blue = replaceLSB(image(i, 0)->Blue, ch);
                    ++i;
                }
            }
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
        if (!image.ReadFromFile(inputImageFile))
        {
            std::cerr << "Erro ao abrir a imagem de entrada." << std::endl;
            return false;
        }

        std::ofstream textFileStream(outputTextFile);
        if (!textFileStream)
        {
            std::cerr << "Erro ao criar o arquivo de texto." << std::endl;
            return false;
        }

        char ch = 0;
        for (int i = 0; i < image.TellWidth(); ++i)
        {
            // Itera pelos pixels da imagem e extrai os bits menos significativos para formar a mensagem
            for (int j = 0; j < 3; ++j)
            { // R, G, B
                if (i < image.TellWidth())
                {
                    ch = (ch << 1) | (image(i, 0)->Red & 1);
                    ch = (ch << 1) | (image(i, 0)->Green & 1);
                    ch = (ch << 1) | (image(i, 0)->Blue & 1);
                    ++i;
                }
            }
            if (ch == '\0')
            {
                break; // Termina a leitura quando encontrar o caractere nulo
            }
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

    // Função para substituir o bit menos significativo de um byte com um novo bit
    uint8_t replaceLSB(uint8_t byte, char newBit)
    {
        return (byte & 0xFE) | (newBit - '0');
    }
};

int main()
{
    const char *inputImageFile = "imagem.bmp";
    const char *outputImageFile = "imagem_saida.bmp";
    const char *textFile = "mensagem.txt";
    const char *outputTextFile = "mensagem_saida.txt";

    TextToImageConverter converter(inputImageFile, outputImageFile, textFile, outputTextFile);

    // Codifica a mensagem na imagem
    if (converter.encodeTextInImage())
    {
        // Decodifica a mensagem da imagem
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
