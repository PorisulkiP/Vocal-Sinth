#ifndef WAVE_H
#define WAVE_H

//Wav Header
struct wavHeader {
    // Содержит символы "RIFF" в ASCII кодировке
    // (0x52494646 в big-endian представлении)
    char chunkID[4];

    // 36 + subchunk2Size, или более точно:
    // 4 + (8 + subchunk1Size) + (8 + subchunk2Size)
    // Это оставшийся размер цепочки, начиная с этой позиции.
    // Иначе говоря, это размер файла - 8, то есть,
    // исключены поля chunkId и chunkSize.
    long chunkSize;

    // Содержит символы "WAVE"
    // (0x57415645 в big-endian представлении)
    char format[4];

    // Формат "WAVE" состоит из двух подцепочек: "fmt " и "data":
    // Подцепочка "fmt " описывает формат звуковых данных:

    // Содержит символы "fmt "
    // (0x666d7420 в big-endian представлении)
    char subchunk1ID[4]; //"fmt " = 0x20746D66

    // 16 для формата PCM.
    // Это оставшийся размер подцепочки, начиная с этой позиции.
    long subchunk1Size;

    // Аудио формат, полный список можно получить здесь http://audiocoding.ru/wav_formats.txt
    // Для PCM = 1 (то есть, Линейное квантование).
    // Значения, отличающиеся от 1, обозначают некоторый формат сжатия.
    short audioFormat;

    // Количество каналов. Моно = 1, Стерео = 2 и т.д.
    short numChannels;

    // Частота дискретизации. 8000 Гц, 44100 Гц и т.д.
    long sampleRate;

    // sampleRate * numChannels * bitsPerSample/8
    long byteRate;

    // numChannels * bitsPerSample/8
    // Количество байт для одного сэмпла, включая все каналы.
    short blockAlign;

    // Так называемая "глубиная" или точность звучания. 8 бит, 16 бит и т.д.
    short bitsPerSample;
};

//Chunks
struct chunk_t
{
    char ID[4]; //"data" = 0x61746164
    unsigned long size;  //Chunk data bytes
};

enum wavFormat {
    PulseCodeModulation = 0x01,
    IEEEFloatingPoint = 0x03,
    ALaw = 0x06,
    MuLaw = 0x07,
    IMAADPCM = 0x11,
    YamahaITUG723ADPCM = 0x16,
    GSM610 = 0x31,
    ITUG721ADPCM = 0x40,
    MPEG = 0x50,
    Extensible = 0xFFFE
};

enum samplerate {
    SR_8khz = 8000,
    SR_11khz = 11025,
    SR_12khz = 12000,
    SR_16khz = 16000,
    SR_22khz = 22050,
    SR_24khz = 24000,
    SR_32khz = 32000,
    SR_44khz = 44100,
    SR_48khz = 48000
};


#endif // WAVE_H
