/*
 * File: Histogram_cross_test.cpp
 * Task: Histogram — структура для статистичної обробки даних (завдання 13)
 * Group: К-24
 * Author: Курлов Микита
 * Date: 2026-05-19
 * Description: Крос-тест для порівняння результатів обчислень C-версії (структура)
 *              та C++ версії (клас). Забезпечує, що обидві реалізації працюють
 *              абсолютно ідентично на одних і тих самих вхідних даних.
 */

#include <iostream>
#include <cmath>
#include <iomanip>
#include <cstdlib>

// Підключаємо C-версію (у глобальному просторі імен)
extern "C" {
    #include "../Histogram_C/Histogram.h"
}

// Підключаємо C++ версію (в просторі імен cpp)
#include "Histogram.h"

#define EPS 1e-6

bool approx_equal(double a, double b) {
    if (std::isnan(a) && std::isnan(b)) return true;
    return std::fabs(a - b) < EPS;
}

int main() {
    std::cout << "=========================================================\n";
    std::cout << "        КРОС-ТЕСТ: ПОРІВНЯННЯ C ТА C++ РЕАЛІЗАЦІЙ         \n";
    std::cout << "=========================================================\n\n";

    // 1. Ініціалізація
    Histogram h_c;
    if (Histogram_init(&h_c, 8, -10.0, 10.0) != 0) {
        std::cerr << "[FAIL] Помилка ініціалізації C-структури.\n";
        return 1;
    }

    cpp::Histogram h_cpp(8, -10.0, 10.0);

    // 2. Генерація та додавання тестових даних (50 чисел)
    double test_data[50];
    std::srand(42); // фіксоване зерно для повторюваності
    for (int i = 0; i < 50; i++) {
        // Суміш даних: нормальні в межах [-10, 10], та деякі виходи за межі
        test_data[i] = -12.0 + (std::rand() % 240) * 0.1;
    }

    // Додамо половину в clip_mode = 1, іншу половину в clip_mode = 2
    for (int i = 0; i < 25; i++) {
        addNumber(&h_c, test_data[i], 1);
        h_cpp.addNumber(test_data[i], 1);
    }
    for (int i = 25; i < 50; i++) {
        addNumber(&h_c, test_data[i], 2);
        h_cpp.addNumber(test_data[i], 2);
    }

    // 3. Порівняння базових полів
    bool passed = true;

    if (num(&h_c) != h_cpp.num()) {
        std::cout << "[FAIL] Загальна кількість елементів відрізняється: C = " 
                  << num(&h_c) << ", C++ = " << h_cpp.num() << "\n";
        passed = false;
    }

    for (unsigned i = 0; i < 8; i++) {
        if (numHist(&h_c, i) != h_cpp.numHist(i)) {
            std::cout << "[FAIL] Частота у стовпці " << i << " відрізняється: C = " 
                      << numHist(&h_c, i) << ", C++ = " << h_cpp.numHist(i) << "\n";
            passed = false;
        }
    }

    // 4. Порівняння статистичних показників
    double mean_c = mean(&h_c);
    double mean_cpp = h_cpp.mean();
    if (!approx_equal(mean_c, mean_cpp)) {
        std::cout << "[FAIL] Середнє (mean) відрізняється: C = " << mean_c << ", C++ = " << mean_cpp << "\n";
        passed = false;
    }

    double med_c = median(&h_c);
    double med_cpp = h_cpp.median();
    if (!approx_equal(med_c, med_cpp)) {
        std::cout << "[FAIL] Медіана (median) відрізняється: C = " << med_c << ", C++ = " << med_cpp << "\n";
        passed = false;
    }

    double dev_c = dev(&h_c);
    double dev_cpp = h_cpp.dev();
    if (!approx_equal(dev_c, dev_cpp)) {
        std::cout << "[FAIL] Середнє відхилення (dev) відрізняється: C = " << dev_c << ", C++ = " << dev_cpp << "\n";
        passed = false;
    }

    double var_c = variance(&h_c);
    double var_cpp = h_cpp.variance();
    if (!approx_equal(var_c, var_cpp)) {
        std::cout << "[FAIL] Дисперсія (variance) відрізняється: C = " << var_c << ", C++ = " << var_cpp << "\n";
        passed = false;
    }

    double skew_c = skewness(&h_c);
    double skew_cpp = h_cpp.skewness();
    if (!approx_equal(skew_c, skew_cpp)) {
        std::cout << "[FAIL] Асиметрія (skewness) відрізняється: C = " << skew_c << ", C++ = " << skew_cpp << "\n";
        passed = false;
    }

    double kurt_c = kurtosis(&h_c);
    double kurt_cpp = h_cpp.kurtosis();
    if (!approx_equal(kurt_c, kurt_cpp)) {
        std::cout << "[FAIL] Ексцес (kurtosis) відрізняється: C = " << kurt_c << ", C++ = " << kurt_cpp << "\n";
        passed = false;
    }

    double ent_c = entropy(&h_c);
    double ent_cpp = h_cpp.entropy();
    if (!approx_equal(ent_c, ent_cpp)) {
        std::cout << "[FAIL] Ентропія (entropy) відрізняється: C = " << ent_c << ", C++ = " << ent_cpp << "\n";
        passed = false;
    }

    // 5. Порівняння статистичних тестів
    if (testUniform(&h_c) != h_cpp.testUniform()) {
        std::cout << "[FAIL] testUniform відрізняється: C = " << testUniform(&h_c) 
                  << ", C++ = " << h_cpp.testUniform() << "\n";
        passed = false;
    }

    if (testNormal(&h_c) != h_cpp.testNormal()) {
        std::cout << "[FAIL] testNormal відрізняється: C = " << testNormal(&h_c) 
                  << ", C++ = " << h_cpp.testNormal() << "\n";
        passed = false;
    }

    if (testExponential(&h_c) != h_cpp.testExponential()) {
        std::cout << "[FAIL] testExponential відрізняється: C = " << testExponential(&h_c) 
                  << ", C++ = " << h_cpp.testExponential() << "\n";
        passed = false;
    }

    if (testBinomial(&h_c, 10, 0.5) != h_cpp.testBinomial(10, 0.5)) {
        std::cout << "[FAIL] testBinomial відрізняється: C = " << testBinomial(&h_c, 10, 0.5) 
                  << ", C++ = " << h_cpp.testBinomial(10, 0.5) << "\n";
        passed = false;
    }

    if (testPoisson(&h_c, 4.0) != h_cpp.testPoisson(4.0)) {
        std::cout << "[FAIL] testPoisson відрізняється: C = " << testPoisson(&h_c, 4.0) 
                  << ", C++ = " << h_cpp.testPoisson(4.0) << "\n";
        passed = false;
    }

    // 6. Вердикт
    if (passed) {
        std::cout << "=========================================================\n";
        std::cout << " [SUCCESS] КРОС-ТЕСТ ПРОЙДЕНО УСПІШНО!                   \n";
        std::cout << " C та C++ версії дають абсолютно ідентичні результати.   \n";
        std::cout << "=========================================================\n";
    } else {
        std::cout << "=========================================================\n";
        std::cout << " [ERROR] КРОС-ТЕСТ ПРОВАЛЕНО! Виявлено розбіжності.      \n";
        std::cout << "=========================================================\n";
    }

    Histogram_free(&h_c);
    return passed ? 0 : 1;
}
