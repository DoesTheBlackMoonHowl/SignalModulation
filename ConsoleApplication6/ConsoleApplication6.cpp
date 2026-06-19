#include <iostream>
#include <fstream>
#include <cstdio>
#include <random>
#include <vector>

const double PI = 3.14159265358979323846;

using namespace std;

class SignalGenerator {
private:
    double modulating;
    double carrier;
    double duration;
    double timeStep;
    double modulationDepth;

    double noiseAmplitude;
    bool addNoise;
    mt19937 rng;
    normal_distribution<double> noiseDist;

public:
    SignalGenerator(double modFreq = 1000,
        double carFreq = 1000000,
        double dur = 0.001,
        double step = 0.0000001,
        double modDepth = 1, //0.8
        double noiseAmp = 0.1)
        : modulating(modFreq), carrier(carFreq),
        duration(dur), timeStep(step), modulationDepth(modDepth),
        noiseAmplitude(noiseAmp), addNoise(true),
        rng(random_device{}()), noiseDist(0.0, 1.0) {}

    double modulatingSignal(double t) {
        return sin(2 * PI * modulating * t);
    }

    double carrierSignal(double t) {
        return sin(2 * PI * carrier * t);
    }

    double amplitudeModulatedSignal(double t) {
        double modulating = modulatingSignal(t);
        double carrier = carrierSignal(t);
        //s(t) = [1 + m*f(t)] * c(t)
        return (1.0 + modulationDepth * modulating) * carrier;
    }

    double whiteNoise() {
        return noiseAmplitude * noiseDist(rng);
    }

    double noisyAMSignal(double t) {
        double signal = amplitudeModulatedSignal(t);
        if (addNoise) {
            signal += whiteNoise();
        }
        return signal;
    }

    void generateData() {
        ofstream modFile("modulating.dat");
        ofstream carFile("carrier.dat");
        ofstream amFile("am_signal.dat");
        ofstream noiseFile("noise.dat");

        int N = static_cast<int>(duration / timeStep);

        vector<double> noiseData(N);
        for (int i = 0; i < N; i++) {
            noiseData[i] = whiteNoise();
        }

        for (int i = 0; i < N; i++) {
            double t = i * timeStep;
            double tMs = t * 1000.0;

            modFile << tMs << " " << modulatingSignal(t) << endl;
            carFile << tMs << " " << carrierSignal(t) << endl;

            double amSignal = amplitudeModulatedSignal(t);
            double noisySignal = amSignal + (addNoise ? noiseData[i] : 0.0);

            amFile << tMs << " " << noisySignal << endl;
            noiseFile << tMs << " " << noiseData[i] << endl;
        }

        modFile.close();
        carFile.close();
        amFile.close();
        noiseFile.close();
    }

    void createScript() {
        ofstream script("plot_script.gnu");

        script << "set terminal wxt size 1400,1000 enhanced font 'Arial,10'\n";
        script << "set multiplot layout 4,1 title 'Амплитудная модуляция сигнала";
        if (addNoise) {
            script << " (с шумом)";
        }
        script << "' font 'Arial,14'\n\n";

        script << "set yrange [" << -2 << ":" << 2 << "]\n";
        script << "set xlabel 'Время (мс)' font 'Arial,11'\n";
        script << "set ylabel 'Амплитуда' font 'Arial,11'\n";
        script << "set title 'Модулирующий сигнал (f = " << modulating / 1000.0
            << " кГц)' font 'Arial,12'\n";
        script << "set grid\n";
        script << "plot 'modulating.dat' with lines linewidth 2 linecolor rgb 'blue' title 'Модулирующий сигнал'\n\n";

        script << "set yrange [" << -2 << ":" << 2 << "]\n";
        script << "set xlabel 'Время (мс)' font 'Arial,11'\n";
        script << "set ylabel 'Амплитуда' font 'Arial,11'\n";
        script << "set title 'Несущий сигнал (f = " << carrier / 1000000.0
            << " МГц)' font 'Arial,12'\n";
        script << "set grid\n";
        script << "plot 'carrier.dat' with lines linewidth 1 linecolor rgb 'red' title 'Несущий сигнал'\n\n";

        if (addNoise) {
            script << "set yrange [" << -2 << ":" << 2 << "]\n";
            script << "set xlabel 'Время (мс)' font 'Arial,11'\n";
            script << "set ylabel 'Амплитуда' font 'Arial,11'\n";
            script << "set title 'Белый шум (амплитуда = " << noiseAmplitude << ")' font 'Arial,12'\n";
            script << "set grid\n";
            script << "plot 'noise.dat' with lines linewidth 0.5 linecolor rgb 'gray' title 'Белый шум'\n\n";
        }

        script << "set yrange [" << -2 << ":" << 2 << "]\n";
        script << "set xlabel 'Время (мс)' font 'Arial,11'\n";
        script << "set ylabel 'Амплитуда' font 'Arial,11'\n";
        script << "set title 'АМ сигнал (глубина модуляции = " << (modulationDepth * 100.0) << "%)";
        if (addNoise) {
            script << " + белый шум";
        }
        script << "' font 'Arial,12'\n";
        script << "set grid\n";

        //script << "plot 'noise.dat' with lines linewidth 0.5 linecolor rgb 'gray' title 'Белый шум', \\\n";
        //  script << " 'am_signal.dat' with lines linewidth 1 linecolor rgb 'dark-green' title 'АМ сигнал";

        script << "plot 'am_signal.dat' with lines linewidth 1 linecolor rgb 'dark-green' title 'АМ сигнал";
        if (addNoise) {
            script << " + шум";
        }
        script << "'\n\n";

        script << "unset multiplot\n";
        script << "pause -1 'Нажмите Enter для выхода'\n";

        script.close();
    }

    void visualize() {
        generateData();
        createScript();
        ofstream batFile("run_gnuplot.bat");
        batFile << "@echo off\n";
        batFile << "if exist gnuplot\\bin\\gnuplot.exe (\n";
        batFile << "    gnuplot\\bin\\gnuplot.exe plot_script.gnu\n";
        batFile << ") else if exist gnuplot.exe (\n";
        batFile << "    gnuplot.exe plot_script.gnu\n";
        batFile << ") else (\n";
        batFile << "        echo ОШИБКА: gnuplot не найден!\n";
        batFile << "    )\n";
        batFile << ")\n";
        batFile.close();

        system("run_gnuplot.bat");
        //system("gnuplot plot_script.gnu");
    }

    void setModulatingFreq(double freq) {modulating = freq;}
    void setCarrierFreq(double freq) {carrier = freq;}
    void setDuration(double dur) {duration = dur;}
    void setTimeStep(double step) {timeStep = step;}
    void setModulationDepth(double depth) {modulationDepth = depth;}
    void setNoiseAmplitude(double amp) {noiseAmplitude = amp;}
    void setAddNoise(bool flag) {addNoise = flag; }

    double getModulatingFreq() const {return modulating;}
    double getCarrierFreq() const {return carrier;}
    double getDuration() const {return duration;}
    double getTimeStep() const {return timeStep;}
    double getModulationDepth() const { return modulationDepth; }
    double getNoiseAmplitude() const { return noiseAmplitude; }
    bool getAddNoise() const { return addNoise; }
};

void printMenu() {
    cout << "\n==========================================\n";
    cout << "  ГЕНЕРАТОР АМ СИГНАЛОВ\n";
    cout << "==========================================\n";
    cout << "1.  Установить частоту модулирующего сигнала (Гц)\n";
    cout << "2.  Установить несущую частоту (Гц)\n";
    cout << "3.  Установить длительность сигнала (мс)\n";
    cout << "4.  Установить дискретность по времени (мкс)\n";
    cout << "5.  Установить глубину модуляции (0-100%)\n";
    cout << "6.  Установить амплитуду шума\n";
    cout << "7.  Включить/выключить белый шум\n";
    cout << "8.  Показать текущие параметры\n";
    cout << "9.  Сгенерировать и отобразить графики\n";
    cout << "0.  Выход\n";
    cout << "==========================================\n";
    cout << "Выберите действие: ";
}

void displayParameters(const SignalGenerator& sg) {
    cout << "\n--- Текущие параметры ---\n";
    cout << " Частота модулирующего сигнала: " << sg.getModulatingFreq() << " Гц\n";
    cout << " Несущая частота: " << sg.getCarrierFreq()<< " Гц\n";
    cout << " Длительность сигнала: " << sg.getDuration() * 1000.0 << " мс\n";
    cout << " Дискретность по времени: " << sg.getTimeStep() * 1000000.0 << " мкс\n";
    cout << " Глубина модуляции: " << sg.getModulationDepth() * 100.0 << " %\n";
    cout << " Амплитуда белого шума: " << sg.getNoiseAmplitude() << "\n";
    cout << " Белый шум: " << (sg.getAddNoise() ? "ВКЛЮЧЕН" : "ВЫКЛЮЧЕН") << "\n";
    cout << "------------------------\n";
}

int main() {
    setlocale(LC_ALL, "Russian");
    system("chcp 65001 > nul");

    SignalGenerator generator;
    int choice;
    double value;

    while (true) {
        printMenu();
        cin >> choice;

        switch (choice) {
        case 1:
            cout << "Введите частоту модулирующего сигнала (Гц): ";
            cin >> value;
            generator.setModulatingFreq(value);
            cout << "Частота установлена: " << value << " Гц\n";
            break;

        case 2:
            cout << "Введите несущую частоту (Гц): ";
            cin >> value;
            generator.setCarrierFreq(value);
            cout << "Несущая частота установлена: " << value << " Гц\n";
            break;

        case 3:
            cout << "Введите длительность сигнала (мс) [не менее 1]: ";
            cin >> value;
            if (value < 1) {
                cout << "Длительность увеличена до 1 мс\n";
                value = 1;
            }
            generator.setDuration(value / 1000.0);
            cout << "Длительность установлена: " << value << " мс\n";
            break;

        case 4:
            cout << "Введите дискретность по времени (мкс) [не более 0.1]: ";
            cin >> value;
            if (value > 0.1) {
                cout << "Шаг уменьшен до 0.1 мкс\n";
                value = 0.1;
            }
            generator.setTimeStep(value / 1000000.0);
            cout << "Дискретность установлена: " << value << " мкс\n";
            break;

        case 5:
            cout << "Введите глубину модуляции (0-100%): ";
            cin >> value;
            if (value < 0 || value > 100) {
                cout << "Значение должно быть от 0 до 100!\n";
            }
            else {
                generator.setModulationDepth(value / 100.0);
                cout << "Глубина модуляции установлена: " << value << "%\n";
            }
            break;

        case 6:
            cout << "Введите амплитуду белого шума [не меньше 0]: ";
            cin >> value;
            if (value < 0) {
                cout << "Значение должно быть неотрицательным!\n";
            }
            else {
                generator.setNoiseAmplitude(value);
                cout << "Амплитуда шума установлена: " << value << "\n";
            }
            break;

        case 7:
            generator.setAddNoise(!generator.getAddNoise());
            cout << "Белый шум "
                << (generator.getAddNoise() ? "ВКЛ" : "ВЫКЛ") << "\n";
            break;

        case 8:
            displayParameters(generator);
            break;

        case 9:
            cout << "\nГенерация сигналов...\n";
            displayParameters(generator);
            generator.visualize();
            break;

        case 0:
            cout << "Выход из программы.\n";
            return 0;

        default:
            cout << "Неверный выбор!\n";
        }
    }

    return 0;
}
