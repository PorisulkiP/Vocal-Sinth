#include "wave.h"

Wave::Wave(const std::string filepath){}

Wave::~Wave(){
    // чистим за собой
    alDeleteSources(1, &source);
    alDeleteBuffers(1, &buffer);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    alcCloseDevice(device);
    alutExit();
}

void Wave::reportError()
{
    qDebug() << "Error";
    fprintf (stderr, "ALUT error: %s\n", alutGetErrorString(alutGetError()));
    exit (EXIT_FAILURE);
}

void Wave::openFile(const std::string filepath) {
//    byte *buffer = new byte[4];
    std::size_t ext = filepath.find(".mp3");
    if (ext!=std::string::npos)
        // Запуск функции преобразования mp3 в wav

    getWavData(filepath.data());


//    std::string tag = "RIFF";
//    // Читаются первые символы файла
//    // Если это wav, то считываем данные
//    if (tag == "RIFF") {
//        getWavData(filepath.data());
//    }
//    // Если это ogg, то переводим в wav
//    else if (tag == "OggS") {
//        oggToWav(filepath);
//    }
//    // Если это flac, то переводим в wav
//    else if (tag == "fLaC") {
//        flacToWav(filepath);
//    }
}


void Wave::getWavData(const char *fileName)
{
    FILE* fin = fopen(fileName, "rb");

    // Объявляется структура с заголовком
    wav_header header;
    // Считывается заголовок WAV файла
    fread(&header, sizeof(header), 1, fin);

    // Переход на часть с данными
    while (true)
    {
        fread(&header, sizeof(header), 1, fin);
        if (*(unsigned int*)&header.subchunk2Size == 0x61746164)
            break;
        fseek(fin, header.subchunk2Size, SEEK_CUR);
    }

    if (header.audioFormat > 2){
        qDebug("Больше 16 бит");
        return;
    }

    format = to_al_format(header.numChannels, header.audioFormat);

    // Размер и число семплов
    sample_size = header.bitsPerSample / 8;
    samples_count = header.subchunk2Size * 8 / header.bitsPerSample;

    char *value = new char[samples_count];
    memset(value, 0, sizeof(char) * samples_count);

    // Длинна аудиофайла в секундах
    duration = samples_count / header.sampleRate;
    // пиковые значения громкости
    peak = pow(256, sampwidth) / 2;
    // семплы в секунду
    int samplePerSecond = samples_count / duration;
    // переменная для оптимизации хранения данных аудио
    // чем она больше, тем больше отсекается данных
    int optimizer = 20;

    // Чтение бит аудиофайла
    for (int i = 0; i < samples_count; i++)
    {
        fread(&value[i], sample_size, 1, fin);
    }
    fclose(fin);


    for (int i = 0, e = 0, n = 0; i < samples_count; i += optimizer, e++)
    {
        if(e >= samplePerSecond/1000){ // переводятся секунды в миллисекунды
            e = 0;
            n++;
        }
        // Расчитывается сила звука в децибелах
        samples.push_back(MusicMath::linearToDecibel((float)value[i]/peak));
    }
}

ALenum Wave::to_al_format(short channels, short _bits)
{
    bool stereo = (channels > 1);

    switch (_bits) {
    case 1:
        bits = 8;
        break;
    case 2:
        bits = 16;
        break;
    case 3:
        bits = 32;
        break;
    case 4:
        bits = 64;
        break;
    default:
        break;
    }

    switch (_bits) {
    // Если битность больше 16, то переводим в 16 бит
    case 64:
        if (stereo)
            return AL_FORMAT_STEREO16;
        else
            return AL_FORMAT_MONO16;
    case 32:
        if (stereo)
            return AL_FORMAT_STEREO16;
        else
            return AL_FORMAT_MONO16;
    case 16:
        if (stereo)
            return AL_FORMAT_STEREO16;
        else
            return AL_FORMAT_MONO16;
    case 8:
        if (stereo)
            return AL_FORMAT_STEREO8;
        else
            return AL_FORMAT_MONO8;
    default:
        return -1;
    }
}

