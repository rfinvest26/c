/*
 * File: Histogram_test.cpp
 * Task: Histogram — структура для статистичної обробки даних (завдання 13)
 * Group: К-24
 * Author: Курлов Микита
 * Date: 2026-05-19
 * Description: Тестовий файл для перевірки всіх методів C++ класу Histogram.
 *              Містить індивідуальні юніт-тести для кожного методу та
 *              інтерактивний інтерфейс для користувача.
 */

#include "Histogram.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <string>
#include <cstdlib>

#define EPS 1e-5

/* ========================================================================= *
 *                              ЮНІТ-ТЕСТИ                                   *
 * ========================================================================= */

// Тест 1: Конструктор та деструктор
void test_constructor_destructor() {
    {
        cpp::Histogram h(10, -5.0, 5.0);
        if (h.num() != 0) {
            std::cout << "Test 1 FAILED: Total not initialized to 0.\n";
            return;
        }
        for (unsigned i = 0; i < 10; i++) {
            if (h.numHist(i) != 0) {
                std::cout << "Test 1 FAILED: Bin frequency not initialized to 0.\n";
                return;
            }
        }
    } // перевіряємо, що деструктор не ламає пам'ять
    
    // Перевірка некоректних даних
    cpp::Histogram h2(0, -5.0, 5.0); // має переналаштуватись на 10 за замовчуванням
    if (h2.num() != 0) {
        std::cout << "Test 1 FAILED: Invalid M handling.\n";
        return;
    }
    
    cpp::Histogram h3(5, 5.0, -5.0); // має переналаштуватись на [0, 100]
    if (h3.num() != 0) {
        std::cout << "Test 1 FAILED: Invalid limits handling.\n";
        return;
    }

    std::cout << "Test 1 (Constructor and Destructor) passed\n";
}

// Тест 2: Сетери
void test_setters() {
    cpp::Histogram h(5, 0.0, 10.0);
    
    h.setMin(-2.0);
    h.setMax(12.0);
    
    // Додамо елементи, щоб перевірити, що setM очищає частоти
    h.addNumber(5.0, 1);
    h.setM(8);
    if (h.num() != 0 || h.numHist(0) != 0) {
        std::cout << "Test 2 FAILED: setM did not clear histogram fields correctly.\n";
        return;
    }
    
    std::cout << "Test 2 (Setters) passed\n";
}

// Тест 3: Додавання чисел
void test_add_number() {
    cpp::Histogram h(3, 0.0, 6.0); // біни: [0, 2), [2, 4), [4, 6]
    
    // Режим 1: Ігнорування поза межами
    h.addNumber(-1.0, 1);
    h.addNumber(7.0, 1);
    if (h.num() != 0) {
        std::cout << "Test 3 FAILED: Out-of-bounds numbers were not ignored in clip_mode = 1.\n";
        return;
    }
    
    // Додавання нормальних чисел
    h.addNumber(1.0, 1); // бін 0
    h.addNumber(3.0, 1); // бін 1
    h.addNumber(5.0, 1); // бін 2
    
    if (h.numHist(0) != 1 || h.numHist(1) != 1 || h.numHist(2) != 1 || h.num() != 3) {
        std::cout << "Test 3 FAILED: In-bounds numbers were not placed in correct bins.\n";
        return;
    }
    
    // Режим 2: Додавання до крайніх стовпців
    h.addNumber(-1.0, 2); // має піти в бін 0
    h.addNumber(7.0, 2);  // має піти в бін 2
    
    if (h.numHist(0) != 2 || h.numHist(2) != 2 || h.num() != 5) {
        std::cout << "Test 3 FAILED: Out-of-bounds numbers were not handled correctly in clip_mode = 2.\n";
        return;
    }
    
    std::cout << "Test 3 (Add Number) passed\n";
}

// Тест 4: Додавання масиву (Batch)
void test_add_batch() {
    cpp::Histogram h(3, 0.0, 6.0);
    double vals[5] = {1.0, 3.0, 5.0, -1.0, 7.0};
    
    h.addBatch(vals, 5, 1); // Режим 1
    if (h.num() != 3 || h.numHist(0) != 1 || h.numHist(1) != 1 || h.numHist(2) != 1) {
        std::cout << "Test 4 FAILED: Batch addition in clip_mode = 1 failed.\n";
        return;
    }
    
    std::cout << "Test 4 (Add Batch) passed\n";
}

// Тест 5: Статистичні показники
void test_statistics() {
    cpp::Histogram h(3, 0.0, 6.0); // біни: [0, 2), [2, 4), [4, 6]
    // Середини бінів: 1.0, 3.0, 5.0
    // Додамо числа: одне в бін 0, два в бін 1, два в бін 2
    double vals[5] = {1.0, 2.5, 3.5, 4.5, 5.5};
    h.addBatch(vals, 5, 1);
    
    if (h.num() != 5) {
        std::cout << "Test 5 FAILED: num() returned incorrect value.\n";
        return;
    }
    
    if (h.numHist(0) != 1 || h.numHist(1) != 2 || h.numHist(2) != 2) {
        std::cout << "Test 5 FAILED: numHist() returned incorrect values.\n";
        return;
    }
    
    // mean = 3.4
    double expected_mean = 3.4;
    if (std::fabs(h.mean() - expected_mean) > EPS) {
        std::cout << "Test 5 FAILED: mean() = " << h.mean() << ", expected = " << expected_mean << "\n";
        return;
    }
    
    // variance = 2.24
    double expected_var = 2.24;
    if (std::fabs(h.variance() - expected_var) > EPS) {
        std::cout << "Test 5 FAILED: variance() = " << h.variance() << ", expected = " << expected_var << "\n";
        return;
    }
    
    // dev = 1.28
    double expected_dev = 1.28;
    if (std::fabs(h.dev() - expected_dev) > EPS) {
        std::cout << "Test 5 FAILED: dev() = " << h.dev() << ", expected = " << expected_dev << "\n";
        return;
    }
    
    // median = 3.5
    double expected_med = 3.5;
    if (std::fabs(h.median() - expected_med) > EPS) {
        std::cout << "Test 5 FAILED: median() = " << h.median() << ", expected = " << expected_med << "\n";
        return;
    }
    
    // skewness = -0.34377
    double expected_skew = -0.343622;
    if (std::fabs(h.skewness() - expected_skew) > EPS) {
        std::cout << "Test 5 FAILED: skewness() = " << h.skewness() << ", expected = " << expected_skew << "\n";
        return;
    }
    
    // kurtosis = -1.153
    double expected_kurt = -1.153;
    if (std::fabs(h.kurtosis() - expected_kurt) > 1e-3) {
        std::cout << "Test 5 FAILED: kurtosis() = " << h.kurtosis() << ", expected = " << expected_kurt << "\n";
        return;
    }
    
    // entropy = 1.0549
    double expected_ent = 1.0549;
    if (std::fabs(h.entropy() - expected_ent) > 1e-3) {
        std::cout << "Test 5 FAILED: entropy() = " << h.entropy() << ", expected = " << expected_ent << "\n";
        return;
    }
    
    std::cout << "Test 5 (Statistics) passed\n";
}

// Тест 6: Робота з файлами
void test_file_io() {
    cpp::Histogram h1(5, -5.0, 5.0);
    double vals[10] = {-4.0, -3.0, -1.0, 0.0, 1.0, 1.5, 2.0, 3.0, 4.0, 4.5};
    h1.addBatch(vals, 10, 1);
    
    // Текстовий файл
    if (!h1.saveToTextFile("test_hist_cpp.txt")) {
        std::cout << "Test 6 FAILED: saveToTextFile returned error.\n";
        return;
    }
    
    cpp::Histogram h2(1, 0, 1);
    if (!h2.loadFromTextFile("test_hist_cpp.txt")) {
        std::cout << "Test 6 FAILED: loadFromTextFile returned error.\n";
        return;
    }
    
    if (h1.num() != h2.num() || h1.numHist(0) != h2.numHist(0)) {
        std::cout << "Test 6 FAILED: Loaded text histogram differs.\n";
        return;
    }
    
    // Бінарний файл
    if (!h1.saveToBinaryFile("test_hist_cpp.bin")) {
        std::cout << "Test 6 FAILED: saveToBinaryFile returned error.\n";
        return;
    }
    
    cpp::Histogram h3(1, 0, 1);
    if (!h3.loadFromBinaryFile("test_hist_cpp.bin")) {
        std::cout << "Test 6 FAILED: loadFromBinaryFile returned error.\n";
        return;
    }
    
    if (h1.num() != h3.num() || h1.numHist(0) != h3.numHist(0)) {
        std::cout << "Test 6 FAILED: Loaded binary histogram differs.\n";
        return;
    }
    
    std::remove("test_hist_cpp.txt");
    std::remove("test_hist_cpp.bin");
    
    std::cout << "Test 6 (File I/O) passed\n";
}

// Тест 7: Зчитування з рядка
void test_input_from_string() {
    cpp::Histogram h(5, 0.0, 10.0);
    const char* str = " 1.5, 2.5; 3.5  8.5   12.0";
    int chars = h.inputFromString(str);
    
    if (chars == 0) {
        std::cout << "Test 7 FAILED: inputFromString returned 0.\n";
        return;
    }
    
    if (h.num() != 4) {
        std::cout << "Test 7 FAILED: inputFromString total count is " << h.num() << " (expected 4).\n";
        return;
    }
    
    std::cout << "Test 7 (Input from String) passed\n";
}

// Тест 8: Перевірка гіпотез
void test_hypotheses() {
    cpp::Histogram h(5, 0.0, 5.0);
    // Рівномірний розподіл
    for (int i = 0; i < 20; i++) {
        h.addNumber(0.5, 1);
        h.addNumber(1.5, 1);
        h.addNumber(2.5, 1);
        h.addNumber(3.5, 1);
        h.addNumber(4.5, 1);
    }
    
    int unif_res = h.testUniform();
    if (unif_res != 1) {
        std::cout << "Test 8 FAILED: testUniform rejected perfectly uniform data.\n";
        return;
    }
    
    std::cout << "Test 8 (Hypotheses Testing) passed\n";
}

void run_all_tests() {
    std::cout << "--- Запуск модульних тестів (C++ версія) ---\n";
    test_constructor_destructor();
    test_setters();
    test_add_number();
    test_add_batch();
    test_statistics();
    test_file_io();
    test_input_from_string();
    test_hypotheses();
    std::cout << "--- Всі C++ юніт-тести завершено ---\n\n";
}

/* ========================================================================= *
 *                              ГОЛОВНА ПРОГРАМА                             *
 * ========================================================================= */

int main() {
    // 1. Запуск модульних тестів
    run_all_tests();
    
    // 2. Демонстраційний режим
    std::cout << "Бажаєте запустити демонстраційний режим C++? (y/n): ";
    char choice_demo;
    if (!(std::cin >> choice_demo) || (choice_demo != 'y' && choice_demo != 'Y')) {
        std::cout << "Вихід з програми.\n";
        return 0;
    }
    
    std::cout << "\nОберіть режим введення даних:\n"
              << "1 - Консольне введення чисел вручну\n"
              << "2 - Зчитування з текстового файлу (Histogram_test.dat)\n"
              << "3 - Зчитування з бінарного файлу\n"
              << "Вибір: ";
    int mode;
    if (!(std::cin >> mode)) {
        std::cout << "Некоректний вибір. Завершення.\n";
        return 1;
    }
    
    unsigned M = 10;
    double min_v = 0.0;
    double max_v = 100.0;
    
    std::cout << "Введіть кількість стовпців (M) [default: 10]: ";
    unsigned temp_M;
    if (std::cin >> temp_M && temp_M > 0) M = temp_M;
    
    std::cout << "Введіть мінімальну межу гістограми [default: 0.0]: ";
    double temp_min;
    if (std::cin >> temp_min) min_v = temp_min;
    
    std::cout << "Введіть максимальну межу гістограми [default: 100.0]: ";
    double temp_max;
    if (std::cin >> temp_max && temp_max > min_v) max_v = temp_max;
    
    cpp::Histogram h(M, min_v, max_v);
    
    std::cout << "Оберіть режим виходу за межі діапазону (clip_mode):\n"
              << "1 - Ігнорувати числа\n"
              << "2 - Додавати до крайніх стовпців\n"
              << "Вибір: ";
    int clip_mode;
    if (!(std::cin >> clip_mode) || (clip_mode != 1 && clip_mode != 2)) {
        clip_mode = 1;
        std::cout << "Вибрано режим за замовчуванням (1 - ігнорувати).\n";
    }
    
    if (mode == 1) {
        std::cout << "\nВводьте числа (розділені пробілом або новим рядком).\n"
                  << "Для закінчення введення введіть будь-який символ, що не є числом (наприклад, 'q'):\n";
        double val;
        int count = 0;
        while (std::cin >> val) {
            h.addNumber(val, clip_mode);
            count++;
        }
        std::cin.clear();
        std::string dummy;
        std::cin >> dummy; // очистити залишковий символ
        std::cout << "Успішно введено чисел з консолі: " << count << "\n";
    } 
    else if (mode == 2) {
        int res = h.addFromTextFile("Histogram_test.dat", clip_mode);
        if (res < 0) {
            // Якщо файл в Histogram_C, але ми в Histogram_CPP, спробуємо знайти його на рівень вище або створити
            res = h.addFromTextFile("../Histogram_C/Histogram_test.dat", clip_mode);
            if (res < 0) {
                std::cout << "Файл Histogram_test.dat не знайдено. Створюємо автоматично...\n";
                std::ofstream df("Histogram_test.dat");
                if (df.is_open()) {
                    double test_vals[25] = {
                        10.5, 15.2, 23.4, 25.0, 31.8, 33.3, 45.0, 47.9, 49.1, 52.0,
                        55.5, 58.0, 62.1, 64.0, 66.8, 70.2, 75.0, 77.3, 81.0, 85.5,
                        90.0, 92.4, 95.0, 98.1, 102.5
                    };
                    for (int i = 0; i < 25; i++) {
                        df << test_vals[i] << "\n";
                    }
                    df.close();
                    res = h.addFromTextFile("Histogram_test.dat", clip_mode);
                }
            }
        }
        if (res >= 0) {
            std::cout << "Успішно зчитано з текстового файлу: " << res << " чисел.\n";
        } else {
            std::cout << "Не вдалося відкрити або зчитати файл Histogram_test.dat\n";
            return 1;
        }
    } 
    else if (mode == 3) {
        std::cout << "Введіть шлях до бінарного файлу: ";
        std::string bin_path;
        if (!(std::cin >> bin_path)) {
            std::cout << "Некоректний шлях.\n";
            return 1;
        }
        int res = h.addFromBinaryFile(bin_path.c_str(), clip_mode);
        if (res >= 0) {
            std::cout << "Успішно зчитано з бінарного файлу: " << res << " чисел.\n";
        } else {
            std::cout << "Не вдалося відкрити або зчитати бінарний файл " << bin_path << "\n";
            return 1;
        }
    } 
    else {
        std::cout << "Невідомий режим.\n";
        return 1;
    }
    
    // 3. Звіт та виведення
    h.printHistogram();
    
    double m = h.mean();
    double med = h.median();
    double d = h.dev();
    double v = h.variance();
    double s = h.skewness();
    double k = h.kurtosis();
    double e = h.entropy();
    
    std::cout << "--- Статистичні характеристики (C++) ---\n"
              << "Кількість елементів:  " << h.num() << "\n"
              << "Середнє значення (mean): " << m << "\n"
              << "Медіана (median):       " << med << "\n"
              << "Середнє відхилення:      " << d << "\n"
              << "Дисперсія (variance):   " << v << "\n"
              << "Асиметрія (skewness):   " << s << "\n"
              << "Ексцес (kurtosis):      " << k << "\n"
              << "Ентропія Шеннона:       " << e << "\n\n";
              
    std::cout << "--- Результати перевірки статистичних гіпотез (chi2, alpha = 0.05) ---\n"
              << "Рівномірний розподіл:     " << (h.testUniform() ? "ПРИЙНЯТО" : "ВІДХИЛЕНО") << "\n"
              << "Нормальний розподіл:      " << (h.testNormal() ? "ПРИЙНЯТО" : "ВІДХИЛЕНО") << "\n"
              << "Експоненційний розподіл:  " << (h.testExponential() ? "ПРИЙНЯТО" : "ВІДХИЛЕНО") << "\n"
              << "Біноміальний (trials=" << M << ", p=0.5): " << (h.testBinomial(M, 0.5) ? "ПРИЙНЯТО" : "ВІДХИЛЕНО") << "\n"
              << "Пуассона (lambda=5.0):    " << (h.testPoisson(5.0) ? "ПРИЙНЯТО" : "ВІДХИЛЕНО") << "\n\n";
              
    std::cout << "Зберігаємо результати у файли...\n";
    h.saveToTextFile("histogram_cpp_saved.txt");
    h.saveToBinaryFile("histogram_cpp_saved.bin");
    
    std::ofstream rf("histogram_cpp_report.txt");
    if (rf.is_open()) {
        rf << "=== ЗВІТ ПРО СТАТИСТИЧНУ ОБРОБКУ ДАНИХ (C++) ===\n\n"
           << "Гістограма: діапазон [" << min_v << ", " << max_v << "], стовпців: " << M << ", режим clip_mode: " << clip_mode << "\n"
           << "Загальна кількість чисел: " << h.num() << "\n\n"
           << "--- Статистичні характеристики ---\n"
           << "Середнє значення (mean): " << m << "\n"
           << "Медіана (median):       " << med << "\n"
           << "Середнє відхилення:      " << d << "\n"
           << "Дисперсія (variance):   " << v << "\n"
           << "Асиметрія (skewness):   " << s << "\n"
           << "Ексцес (kurtosis):      " << k << "\n"
           << "Ентропія Шеннона:       " << e << "\n\n"
           << "--- Перевірка статистичних гіпотез (рівень 0.05) ---\n"
           << "Рівномірний розподіл:     " << (h.testUniform() ? "ПРИЙНЯТО" : "ВІДХИЛЕНО") << "\n"
           << "Нормальний розподіл:      " << (h.testNormal() ? "ПРИЙНЯТО" : "ВІДХИЛЕНО") << "\n"
           << "Експоненційний розподіл:  " << (h.testExponential() ? "ПРИЙНЯТО" : "ВІДХИЛЕНО") << "\n";
        rf.close();
        std::cout << "Звіт збережено у 'histogram_cpp_report.txt'.\n";
    }
    
    return 0;
}
