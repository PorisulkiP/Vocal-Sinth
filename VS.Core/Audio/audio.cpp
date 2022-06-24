#include "audio.h"

//#define DEBUG

Audio::Audio():stereo(false){}

Audio::Audio(const string &path){ openFile(path); }

bool Audio::openFile(const string &path){
    // Переводится string в const char *
    setFileName(path);

    // Получаются данные об аудиофайле
    if (!getWaveData()) {
        return false;
    }

    // Устанавливается буффер
    if(format == AL_FORMAT_MONO8 || format == AL_FORMAT_STEREO8) {
       setBuffers(_dataChar);
    }
    else if(format == AL_FORMAT_MONO16 || format == AL_FORMAT_STEREO16) {
        setBuffers(_data16bit);
    }
    else if(format == AL_FORMAT_MONO_FLOAT32 ||
            format == AL_FORMAT_STEREO_FLOAT32) {
        setBuffers(_datafloat);
    }

    #ifdef DEBUG
            qDebug() << "fileName" << fileName.data();
            qDebug() << "depth" << depth();
    #endif

    return true;
}

Audio::~Audio(){
    // Удаляются все буферы
    alDeleteBuffers(1, &alBuffer);
    alDeleteBuffers(1, &alBufferRight);
    alDeleteBuffers(1, &alBufferLeft);

    // Очищаются все созданые массивы
    pcm_charBuffer.clear();
    pcm_16buffer.clear();

    _dataChar.clear();
    _data16bit.clear();
    _datafloat.clear();

    _LSamples.clear();
    _RSamples.clear();

    _DbL.clear();
    _DbR.clear();
}

template<class T>
void Audio::setBuffers(vector<T> &data){
    alGenBuffers(1, &alBuffer); // Создание буфера для звука
    // Создаются буферы для разных каналов аудиофайла
    if (header.numChannels == 1) { // Моно звук
        // Загрузка данных в буфер из массива
        alBufferData(alBuffer, format, &data[0], samples_count, header.sampleRate);
        qDebug() << "data.size():" << data.size();
        qDebug() << "samples_count:" << samples_count;
        reportError();
    }
    else { // Стерео звук
        alGenBuffers(1, &alBufferLeft); // Создание буфера для левого канала
        alGenBuffers(1, &alBufferRight); // Создание буфера для правого канала

        // Загрузка данных в буфер из массива
        alBufferData(alBuffer, format, &data[0], samples_count, header.sampleRate);
        // Создание массива для копирования данных
        char* data1 = new char[chunk.size / 2];
        // Копирование данных для левого буфера
        for (auto i=0 ; i < header.sampleRate ; ++i) {
            memcpy(&data1[i*header.bitsPerSample], &data[2*i*header.bitsPerSample], header.bitsPerSample);
        }
        // наполнение буфера данными
        alBufferData(alBufferLeft, toAlMonoFormat(header.audioFormat), data1, samples_count/2, header.sampleRate);
        // Копирование данных для правого буфера из основного массива
        for (auto i=0 ; i < header.sampleRate ; ++i) {
            memcpy(&data1[i*header.bitsPerSample], &data[(2*i+1)*header.bitsPerSample], header.bitsPerSample);
        }
        // наполнение буфера данными
        alBufferData(alBufferRight, toAlMonoFormat(header.audioFormat), data1, samples_count/2, header.sampleRate);
        // Очистка памяти от массива для копирования данных
        delete[] data1;
    }
}

void Audio::reportError()
{
    if (alGetError() != AL_NO_ERROR){
        // Если была почена ошибка в библиотеке ALUT, то ошибка выводится
        qDebug() << "ALUT error:" << alutGetErrorString(alutGetError());
        qDebug() << "OpenAL reported the following error: \n" << alutGetErrorString(alGetError());
    }
}

bool Audio::getWaveData() {
    // Объявляется файл в QFile, поскольку обычный fopen
    // Не работает с иероглифами
    QFile qf(fileName.data());
    // Файл открывается
    qf.open(QIODevice::ReadOnly);
    // Передаётся handle
    int fd = dup(qf.handle());
    qf.close();
    // Считывается файл в бинароном виде
    fin = fdopen(fd, "rb"); // !!! use dup()
    // Если файл не открылся возращается false
    if (fin == NULL) {
        return false;
    }

    // Считывание заголовка WAV файла
    fread(&header, sizeof(header), 1, fin);

    int i = 0;
    // переход к части с данными
    while (i < 1000) {
        // Считывается область размером с chunk
        fread(&chunk, sizeof(chunk), 1, fin);
//        qDebug() << *(unsigned int*)&chunk.ID;
        // Как только каретка доходит до области данных, цикл останавливается
        if (*(unsigned int*)&chunk.ID == 0x61746164){ break; }
        // Каретка передвигается на размер чанка
        fseek(fin, chunk.size, SEEK_CUR);
        i++;
    }
    // Установка флага стерео звучания
    stereo = (numChannels() > 1) ? true: false;


    // Расчитывается размер семплов
    sample_size = header.bitsPerSample / 8;
    //Расчитывается общее кол-во семплов в файле
    samples_count = chunk.size / (header.numChannels * (header.bitsPerSample / 8));

    // Длинна аудиофайла в секундах
    _duration = double(samples_count) / header.sampleRate;

    // пиковые значения громкости
    peak = pow(256, sampwidth) / 2;

    // Получается формат в виде ALenum
    format = toAlFormat(header.bitsPerSample);
    // отправляется исгнал об обновлении данных
    emit dataUpdeted();

    return true;
}

void Audio::readSamples(const vector<char> data) {
    future<void> a1;
    // Считывание данных, если у аудио только один канал
    if (header.numChannels == 1) {
        // _LSamples принимает размер data.size()
        _LSamples.resize(data.size());
        // Запускаются потоки для расчёта
        a1 = async(launch::async, [=]{
            for (int i = 0; i < samples_count; ++i)  {
                _mutex.lock();
                _LSamples.emplace_back(data.at(i));
                // Расчитывается сила звука в децибелах
                if (data.at(i) > 0) {
                    _DbL.append(MusicMath::linearToDecibel(data.at(i)/peak));
                }
                else {
                    _DbL.append(-MusicMath::linearToDecibel(abs(data.at(i))/peak));
                }
                _mutex.unlock();
            }
        });
    }
    else{ // Если у аудио 2 канала
        _LSamples.resize(data.size());
        _RSamples.resize(data.size());
        if (!stereo){
            a1 = async(launch::async, [=]{
                for (int i = 0; i < samples_count; ++i)  {
                    _mutex.lock();
                    _LSamples.emplace_back(data.at(i));
                    // Расчитывается сила звука в децибелах
                    if (data.at(i) > 0) {
                        _DbL.append(MusicMath::linearToDecibel(data.at(i)/peak));
                    }
                    else {
                        _DbL.append(-MusicMath::linearToDecibel(abs(data.at(i))/peak));
                    }
                    _mutex.unlock();
                }
            });
        }
        else{
            a1 = async(launch::async, [=]{
                for (int i = 0; i < samples_count; ++i) {
                    _mutex.lock();
                    if ((i % 2) == 0){
                        _LSamples.emplace_back(data.at(i));
                        // Расчитывается сила звука в децибелах
                        if (data.at(i) > 0) {
                            _DbL.append(MusicMath::linearToDecibel(data.at(int(i))/peak));
                        }
                        else {
                            _DbL.append(-MusicMath::linearToDecibel(abs(data.at(int(i)))/peak));
                        }
                    }
                    else{
                        _RSamples.emplace_back(data.at(i));
                        // Расчитывается сила звука в децибелах
                        if (data.at(i) > 0) {
                            _DbR.append(MusicMath::linearToDecibel(data.at(int(i))/peak));
                        }
                        else {
                            _DbR.append(-MusicMath::linearToDecibel(abs(data.at(int(i)))/peak));
                        }
                    }
                    _mutex.unlock();
                }
            });
        }
    }
    a1.get();
}

ALuint Audio::source() const
{
    return _source;
}

void Audio::setSource(ALuint newSource)
{
    _source = newSource;
}

ALuint Audio::getAlBuffer() const
{
    return alBuffer;
}

void Audio::setAlBuffer(ALuint newAlBuffer)
{
    alBuffer = newAlBuffer;
}

// TODO: Подключить ресемплер для перекодирования из 24-32 битных файлов в 16 бит
ALenum Audio::toAlFormat(short _bits) {
    bits = _bits; // Установка глубины аудиофайла
    switch (_bits) {
    case 8: // Если файл восьмибитный
        _dataChar = readFile();
        readSamples(_dataChar);
        if (stereo)
            return AL_FORMAT_STEREO8;
        else
            return AL_FORMAT_MONO8;
    case 16:
        _data16bit = readINT16File();
        readSamples(int16ToChar(_data16bit));
        if (stereo)
            return AL_FORMAT_STEREO16;
        else
            return AL_FORMAT_MONO16;
    case 24:
        _data16bit = readINT16File();
        readSamples(int16ToChar(_data16bit));
        if (stereo)
            return AL_FORMAT_STEREO16;
        else
            return AL_FORMAT_MONO16;
    case 32:
        _datafloat = readFloatFile();
        readSamples(floatToChar(_datafloat));
        if (stereo)
            return AL_FORMAT_STEREO_FLOAT32;
        else
            return AL_FORMAT_MONO_FLOAT32;
    default:
        return -1;
    }
    return -1;
}

ALenum Audio::toAlMonoFormat(short _bits) {
    switch (_bits) {
    case 8:
        return AL_FORMAT_MONO8;
    case 16:
        return AL_FORMAT_MONO16;
    case 24:
        return AL_FORMAT_MONO16;
    case 32:
        return AL_FORMAT_MONO16;
    default:
        return -1;
    }
    return -1;
}

void Audio::notePreparation()
{
    // Находится нужный oto
    for (const auto& var : VB->getOto())
    {
        if (fileName.find(var->getWav()) != string::npos) { oto = var; break;}
    }
    if (!oto) { return; } // Если файл не найден, то завершаем функцию

    // Начало и конец должны измеряться в семплах
    int start = oto->getOffset() / 1000. * header.sampleRate;
    int end = samplesCount() - abs(oto->getCutoff()) / 1000. * header.sampleRate;

    vector<short> tmp;
    tmp.reserve(end - start);

    for (size_t var = start; var < end; ++var) {
        tmp.emplace_back(_data16bit.at(var));
    }

    _data16bit = tmp;
    tmp.resize(0);

//    qDebug() << "Нота готова" << *oto;
}

void Audio::setNewLength(int newLength)
{
    if (!isReady) // Если нота не обработана
    {
        isReady = true; // Устанавливается флаг готовности ноты
        notePreparation(); // Нота подготавливается к работе
    }
    if (!oto) { return; } // Если файла oto нет, то завершаем функцию

    // Высчитывается новое кол-во образцов для ноты
    // Не знаю, что за 100, но с ним корректно высчитывается кол-во семплов
    qDebug() << "duration:" << duration();
    qDebug() << "gPxInMs:" << gPxInMs;
    qDebug() << "newLength:" << newLength;
    qDebug() << "newLength:" << newLength / gPxInMs;
    int newSampleCount = newLength * header.sampleRate / gPxInMs;
    qDebug() << "newSampleCount:" << newSampleCount;
//    double vel = pow(2.0, (64 - 100) / 64.);
//    qDebug() << vel;

    tmpShort.clear(); // Массив очищается
    tmpShort.reserve(newSampleCount);// Под массив резервируется память

    // Начало и конец должны измеряться в семплах
    int consonantSample = oto->getConsonant() * header.sampleRate / 1000;

    // Данные в oto содержат мс, потому надо доходить до определённых семплов
    // после чего перключаться на другую часть файла, согласно данным в oto
    for (int var = 0; var < consonantSample; ++var)
    {
        // Данные переносятся обходя конструктор копирования в emplace_back
        tmpShort.emplace_back(_data16bit.at(var));
    }
    // Создаётся future<void>, чтобы циклы выполнялись асинхронно в каждом потоке
    future<void> future1;
    // Создаётся вектор потоковы
    vector<std::thread> threads;
    // Создаётся максимальное число потоков для выполнения задачи
    for(size_t i = 0; i < std::thread::hardware_concurrency(); ++i)
    {
        // Создаётся поток с лямбда функцией
        std::thread thr([&]{
            future1 = async(launch::async, [this, consonantSample, newSampleCount] {
                // Начиется растягивание части ноты после консонанты
                for (size_t var = consonantSample; var < newSampleCount;)
                {
                    // включается монопольное использование общих данных для потока
                    _mutex.lock();
                    // Если цикл будет многопоточным, то появятся шумы
                    for (int i = consonantSample; i < _data16bit.size(); ++i)
                    {
                        tmpShort.emplace_back(_data16bit.at(i));
                    }
                    // Производится шаг на размер от consonantSample до  _data16bit.size()
                    var += _data16bit.size() - consonantSample;
                    // отключается монопольное использование общих данных для потока
                    _mutex.unlock();
                }
            });
        });
        // Поток добавляется в вектор
        threads.emplace_back(move(thr));
    }
    for(auto& thr : threads) { thr.join(); } // Все потоки объединяются

    samples_count = newSampleCount; // Установка нового кол-ва семплов
    tmpShort.resize(samples_count); // Обрезаем всё лишнее

    alDeleteBuffers(1, &alBuffer); // Удаляем прошлый буфер
    future1.get(); // получаются данные из future<void>

    // Загрузка данных в буфер из массива
    setBuffers(tmpShort);
    if (!alBuffer) { reportError(); }

    // получается частота дискретизации
    int sampleRate;
    alGetBufferi(alBuffer, AL_FREQUENCY, &sampleRate);
    // Если её не удалось получить, значит буффер не создан,
    // потому функция запускается вновь, но не более 5 раз.
    // Обычно хватает и 1-2 проходов.
//    if (!sampleRate && circle < 10){
//        qDebug() << "circle:" << circle;
//        circle++;
//        setNewLength(newLength+1);
//    }
    circle = 0;
}

// Здесь изменяется громкость конкретной ноты(буффера)
// Этот параметр будет изменять график громкости в редкторе графов
void Audio::setNewVolume(int newVol)
{
     for_each(tmpShort.begin(), tmpShort.end(), [=](auto &temp) {
        double scaledValue = floor((temp * newVol/100) + 0.5);
        if (scaledValue >= INT16_MAX) {
            return temp = INT16_MAX;
        } else if (scaledValue <= INT16_MIN) {
            return temp = INT16_MIN;
        } else {
            return temp = int16_t(scaledValue);
        }
    });
    setBuffers(tmpShort);
}

// Здесь изменяется громкость источника звука
// Этот параметр изменяет ползунок громкости vol_value в partHead
void Audio::setVolume(int volume)
{
    float newVol = volume / 1000.;
    if(newVol >= 0) { alSourcef(_source, AL_GAIN, newVol); gain = newVol;}
    else            { return; }

    if (_DbL.empty()) { return; }
    // Создаются массивы с временной громкостью для двух каналов
    QList<float> tmpL = _DbL;
    QList<float> tmpR = _DbR; // Если звук моно, то список будет пустой
    newVol = volume / 10.;

    // Создаётся future, чтобы циклы выполнялись асинхронно
    future<void> future1;
    if (!stereo) {
        future1 = async(launch::async, [=]{
            unique_lock<mutex> lock(_mutex);
            for(auto &&db : _DbL){ db *= newVol; }
        });
    }
    else {
        future1 = async(launch::async, [=]{
            unique_lock<mutex> lock(_mutex);
            for(auto &&db : _DbL){ db *= newVol; }
            for(auto &&db : _DbR){ db *= newVol; }
        });
    }
    future1.get();

    emit dataUpdeted();

//    qDebug() << "path:" << fileName.data();

    _DbL = tmpL;
    _DbR = tmpR;
}

float Audio::getVolume()
{
    alGetSourcef(_source, AL_GAIN, &gain);
    return gain * 100;
}

void Audio::setMove(int X)
{
    // переделать функцию под изменение громкости правого и левого буфера
    float sourcePosAL[] = {(float)X/100, 0.0f, 0.0f};
    alSourcefv(alBufferLeft, AL_POSITION, sourcePosAL);
    qDebug() << "X:" << sourcePosAL[0];
}

bool Audio::haveChange() const
{
    return _haveChange;
}

void Audio::setHaveChange(bool newHaveChange)
{
    _haveChange = newHaveChange;
}

string Audio::getFileName() const
{
    return fileName;
}

void Audio::setFileName(const string &newFileName)
{
    fileName = newFileName;
}

void Audio::mute()
{
    muted = !muted; // инвертируется флаг отключения звука
    if(muted) // Если звук не отключён
    {
        tmp_gain = gain; // Получается текущая громкость
        alSourcef(_source, AL_GAIN, 0); // Громкость выставляется в 0
    }
    else{ // Если звук отключён
        alSourcef(_source, AL_GAIN, tmp_gain); // Выставляется прошлая громкость
    }
}

void Audio::seekTo(float seek_to)
{
    alSourcef(_source, AL_SEC_OFFSET, seek_to);
}

void Audio::play()
{
    alSourcei(_source, AL_BUFFER, alBuffer); // Установка буфера в источник
    alSourcePlay(_source);
}

void Audio::pause()
{
    alSourcePause(_source);
}

void Audio::stop()
{
    alSourceStop(_source);
}

bool Audio::isPlaying()
{
    alGetSourcei(_source, AL_SOURCE_STATE, &state);
    if (state == AL_PLAYING)    { return true; }
    else                        { return false;}
}

vector<char> Audio::readFile(){
    // Чтение бит аудиофайла
    char temp;
    vector<char> o_data;
    o_data.reserve(sizeof(char) * samples_count);
    for (int i = 0; i < samples_count; i++) {
        fread(&temp, sample_size, 1, fin);
        o_data.emplace_back(move(temp));
    }
    // Закрытие файла
    fclose(fin);
    return o_data;
}

vector<float> Audio::readFloatFile(){
    // Чтение бит аудиофайла
    float temp;
    vector<float> o_data;
    o_data.reserve(sizeof(float) * samples_count);
    for (int i = 0; i < samples_count; ++i) {
        fread(&temp, sample_size, 1, fin);
        o_data.emplace_back(move(temp));
    }

    // Закрытие файла
    fclose(fin);
    return o_data;
}


vector<short> Audio::readINT16File(){
    short temp; // Создаётся временная переменная
    vector<short> o_data; // Создаётся вектор для результата
    o_data.reserve(sizeof(short) * samples_count); // Для вектора резервируется память
    for (int i = 0; i < samples_count; i++) {
        fread(&temp, sample_size, 1, fin); // Считываются биты аудиофайла
        o_data.emplace_back(temp);
    }

    // Закрытие файла
    fclose(fin);

    return o_data;
}

vector<short> Audio::floatTo16bit(vector<float> &_buffer){
    if (!pcm_16buffer.empty())
        pcm_16buffer.clear();
    pcm_16buffer.reserve(_buffer.size());
    foreach (auto &&var, _buffer) {
        // Сэмпл преобразуется в int16
        short tmp = var * 32767 / 32767;
        pcm_16buffer.emplace_back(move(tmp));
    }
    return pcm_16buffer;
}


vector<short> Audio::charTo16bit(vector<char> &_buffer){
    if (!pcm_16buffer.empty())
        pcm_16buffer.clear();
    pcm_16buffer.reserve(_buffer.size());
    for(int i = 0; i < _buffer.size(); ++i)
    {
        // Сэмпл преобразуется в int16
        short tmp = _buffer.at(i) * 32767 / 32767;
        pcm_16buffer.emplace_back(move(tmp));
    }
    return pcm_16buffer;
}

vector<char> Audio::int16ToChar(vector<short> &_buffer){
    if (!pcm_charBuffer.empty())
        pcm_charBuffer.clear();
    pcm_charBuffer.reserve(_buffer.size());
    for(int i = 0; i < _buffer.size(); ++i) {
        _mutex.lock();
        // Сэмпл преобразуется в char
        char tmp = (_buffer.at(i) >> 8) & 0xFFF / 10;
        pcm_charBuffer.emplace_back(move(tmp));
        _mutex.unlock();
    }
    return pcm_charBuffer;
}

vector<char> Audio::floatToChar(vector<float> &_buffer){
    if (!pcm_charBuffer.empty())
        pcm_charBuffer.clear();
    pcm_charBuffer.reserve(_buffer.size());
    foreach (auto var, _buffer) {
        // Сэмпл преобразуется в char
        char tmp = var * 127 / 127;
        pcm_charBuffer.emplace_back(move(tmp));
    }
    return pcm_charBuffer;
}

