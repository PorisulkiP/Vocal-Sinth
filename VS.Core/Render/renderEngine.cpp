#include "renderEngine.h"

RenderEngine::RenderEngine()
{

}


//// Перестраивает преобразование Фурье X путем смещения компонента нулевой частоты в центр массива.
//// 1 2 3 4 5 6 --> 4 5 6 1 2 3
//void RenderEngine::fftShift(vector<double> &plan)
//{
//    // Находится среедина массива
//    auto mid = plan.begin() + (plan.size() >> 1);
//    // Если массив кратен 2, то середина смещается в центр
//    if (!(plan.size() % 2))
//    {
//        rotate(plan.begin(), mid, plan.end());
//    }
//    else // если не кратен, то смещается следующий от 0 элемент
//    {
//        rotate(plan.begin(), mid + 1, plan.end());
//    }
//}

//void RenderEngine::setNewTone(int newTone)
//{
//    if (!oto) {return;} // Если файл oto не найден, то завершаем функцию
//    // Объявляется масив комплексных чисел
//    vector<complex<double>> data;
//    data.reserve(samples_count);// Под массив резервируется память

//    for (auto var : tmpShort) {
//        data.emplace_back(complex<double>(var));
//    }
//    if (data.size() == 0) {return;}

//    vector<double> dataDouble;
//    dataDouble.reserve(samples_count);// Под массив резервируется память

//    // создание одномерного плана БПФ, то есть тут только частота
//    fftw_plan plan = fftw_plan_dft_1d(data.size(), (fftw_complex*) &data[0],
//                                      (fftw_complex*) &data[0], FFTW_FORWARD, FFTW_ESTIMATE);
//    // Выполняем преобразование Фурье (ПФ)
//    fftw_execute(plan);
//    // удаление плана БПФ
//    fftw_destroy_plan(plan);

//    // Собирается массив модулей комплексных чисел
//    for(auto &&var : data)
//    {
//        dataDouble.emplace_back(var.real()); //
//    }

//    //    fftShift(dataDouble); // Ноль сдвигается в центр

//    vector<double> magnitude;
//    //расчитывается магнитуда из преобразования Фурье
//    for (auto i = 0; i < dataDouble.size() / 2 - 1; ++i){
//        auto re = dataDouble.at(2*i);
//        auto im = dataDouble.at(2*i+1);
//        magnitude.emplace_back(sqrt(re*re+im*im));
//    }

//    // Находится пик
//    double max_magnitude = -INFINITY;
//    int max_index = -1;
//    for (auto i = 0; i < magnitude.size(); ++i){
//        if (magnitude.at(i) > max_magnitude){
//            max_magnitude = magnitude.at(i);
//            max_index = i;
//        }
//    }

//    // Пик конвертируется в частоту
//    double freq = max_index * header.sampleRate / dataDouble.size() / 2;
//    qDebug() << *oto;
//    qDebug() << "max_index:" << max_index;
//    qDebug() << "freq:" << freq;

//    // Частота конвертируется в тон
//    int tone = MusicMath::freqToTone(max_index);
//    if(tone == -1 && circle < 2) {
//        circle++;
//        setNewTone(newTone);
//    }
//    circle = 0;
//    qDebug() << "tone:" << tone;
//    qDebug() << "tone:" << MusicMath::noteNameFromNum(tone).data();

//    // Сравнивается высчитаный тон и тон, на котором должны находится нота
//    int toneDiff = newTone - tone;
//    // Вся магнитуда изменяется на разницу тонов
//    qDebug() << "toneDiff" << toneDiff;
//    //    std::for_each(dataDouble.begin(), dataDouble.end(), [=](auto &c) { return c += toneDiff * 100;});
//    // Если разница тонов положительна, до двигаем массив в лево
//    if (toneDiff > 0)
//        rotate(dataDouble.begin(), dataDouble.begin() + toneDiff, dataDouble.end());
//    else{ // если же отрицательтно, то в право
//        rotate(dataDouble.rbegin(), dataDouble.rbegin() - toneDiff, dataDouble.rend());
//    }

//    data.clear(); // Очищается массив комплексных чисел
//    // Магнитуда преобразуется в вектор комплексных чисел
//    for (auto &&var : dataDouble) {
//        data.emplace_back(complex<double>(var));
//    }

//    // Обратное БПФ
//    plan = fftw_plan_dft_1d(data.size(), (fftw_complex*) &data[0],
//                            (fftw_complex*) &data[0], FFTW_BACKWARD, FFTW_ESTIMATE);
//    fftw_execute(plan);
//    fftw_destroy_plan(plan);
//    fftw_cleanup();

//    // Нормализация данных
//    std::for_each(data.begin(), data.end(), [=](auto &c) { return c *= 1. / data.size();});

//    // Установка данных в буффер
//    vector<short> shortNum;

//    for(auto &&var:data) { shortNum.emplace_back(var.real()); }

//    int i = 0;
//    for (int var = 0; var < 100; ++var) {
//        if (shortNum.at(var) != tmpShort.at(var)) { i++;}
//    }
//    qDebug() << "shortNum:" << shortNum.size() << tmpShort.size() << i;

//    setBuffers(shortNum);
//}

