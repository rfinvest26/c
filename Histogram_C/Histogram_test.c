/*
 * File: Histogram_test.c
 * Task: Histogram — структура для статистичної обробки даних (завдання 13)
 * Group: К-24
 * Author: Курлов Микита
 * Date: 2026-05-19
 * Description: Тестовий файл для перевірки всіх функцій C-реалізації гістограми.
 *              Містить індивідуальні юніт-тести для кожної функції та
 *              інтерактивний інтерфейс для користувача.
 */

#include "Histogram.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define EPS 1e-5

/* ========================================================================= *
 *                              ЮНІТ-ТЕСТИ                                   *
 * ========================================================================= */

// Тест 1: Ініціалізація та звільнення пам'яті
void test_init_free(void) {
    Histogram h;
    int res = Histogram_init(&h, 10, -5.0, 5.0);
    if (res != 0) {
        printf("Test 1 FAILED: Histogram_init returned non-zero for valid inputs.\n");
        return;
    }
    if (h.M != 10 || h.min_hist != -5.0 || h.max_hist != 5.0 || h.total != 0 || h.frequency == NULL) {
        printf("Test 1 FAILED: Fields not initialized correctly.\n");
        Histogram_free(&h);
        return;
    }
    
    // Перевірка некоректних даних
    int res2 = Histogram_init(&h, 0, -5.0, 5.0);
    if (res2 == 0) {
        printf("Test 1 FAILED: Histogram_init accepted M = 0.\n");
        return;
    }
    
    int res3 = Histogram_init(&h, 5, 5.0, -5.0);
    if (res3 == 0) {
        printf("Test 1 FAILED: Histogram_init accepted min >= max.\n");
        return;
    }

    Histogram_free(&h);
    printf("Test 1 (Init and Free) passed\n");
}

// Тест 2: Сетери
void test_setters(void) {
    Histogram h;
    Histogram_init(&h, 5, 0.0, 10.0);
    
    setMin(&h, -2.0);
    if (h.min_hist != -2.0) {
        printf("Test 2 FAILED: setMin did not update min_hist correctly.\n");
        Histogram_free(&h);
        return;
    }
    
    setMax(&h, 12.0);
    if (h.max_hist != 12.0) {
        printf("Test 2 FAILED: setMax did not update max_hist correctly.\n");
        Histogram_free(&h);
        return;
    }
    
    // Додамо елементи, щоб перевірити, що setM очищає частоти
    addNumber(&h, 5.0, 1);
    setM(&h, 8);
    if (h.M != 8 || h.total != 0 || h.frequency[0] != 0) {
        printf("Test 2 FAILED: setM did not resize or clear histogram fields correctly.\n");
        Histogram_free(&h);
        return;
    }
    
    Histogram_free(&h);
    printf("Test 2 (Setters) passed\n");
}

// Тест 3: Додавання чисел
void test_add_number(void) {
    Histogram h;
    Histogram_init(&h, 3, 0.0, 6.0); // біни: [0, 2), [2, 4), [4, 6]
    
    // Режим 1: Ігнорування поза межами
    addNumber(&h, -1.0, 1);
    addNumber(&h, 7.0, 1);
    if (h.total != 0) {
        printf("Test 3 FAILED: Out-of-bounds numbers were not ignored in clip_mode = 1.\n");
        Histogram_free(&h);
        return;
    }
    
    // Додавання нормальних чисел
    addNumber(&h, 1.0, 1); // біні 0
    addNumber(&h, 3.0, 1); // біні 1
    addNumber(&h, 5.0, 1); // біні 2
    
    if (h.frequency[0] != 1 || h.frequency[1] != 1 || h.frequency[2] != 1 || h.total != 3) {
        printf("Test 3 FAILED: In-bounds numbers were not placed in correct bins.\n");
        Histogram_free(&h);
        return;
    }
    
    // Режим 2: Додавання до крайніх стовпців
    addNumber(&h, -1.0, 2); // має піти в біні 0
    addNumber(&h, 7.0, 2);  // має піти в біні 2
    
    if (h.frequency[0] != 2 || h.frequency[2] != 2 || h.total != 5) {
        printf("Test 3 FAILED: Out-of-bounds numbers were not handled correctly in clip_mode = 2.\n");
        Histogram_free(&h);
        return;
    }
    
    Histogram_free(&h);
    printf("Test 3 (Add Number) passed\n");
}

// Тест 4: Додавання масиву (Batch)
void test_add_batch(void) {
    Histogram h;
    Histogram_init(&h, 3, 0.0, 6.0);
    double vals[5] = {1.0, 3.0, 5.0, -1.0, 7.0};
    
    addBatch(&h, vals, 5, 1); // Режим 1
    if (h.total != 3 || h.frequency[0] != 1 || h.frequency[1] != 1 || h.frequency[2] != 1) {
        printf("Test 4 FAILED: Batch addition in clip_mode = 1 failed.\n");
        Histogram_free(&h);
        return;
    }
    
    Histogram_free(&h);
    printf("Test 4 (Add Batch) passed\n");
}

// Тест 5: Статистичні показники
void test_statistics(void) {
    Histogram h;
    Histogram_init(&h, 3, 0.0, 6.0); // біни: [0, 2), [2, 4), [4, 6]
    // Середини бінів: 1.0, 3.0, 5.0
    // Додамо числа: одне в біні 0, два в біні 1, два в біні 2
    double vals[5] = {1.0, 2.5, 3.5, 4.5, 5.5};
    addBatch(&h, vals, 5, 1);
    
    if (num(&h) != 5) {
        printf("Test 5 FAILED: num() returned incorrect value.\n");
        Histogram_free(&h);
        return;
    }
    
    if (numHist(&h, 0) != 1 || numHist(&h, 1) != 2 || numHist(&h, 2) != 2) {
        printf("Test 5 FAILED: numHist() returned incorrect values.\n");
        Histogram_free(&h);
        return;
    }
    
    // Еталонні розрахунки на базі середин бінів (1.0, 3.0, 5.0):
    // mean = (1*1.0 + 2*3.0 + 2*5.0) / 5 = 3.4
    double expected_mean = 3.4;
    if (fabs(mean(&h) - expected_mean) > EPS) {
        printf("Test 5 FAILED: mean() = %f, expected = %f\n", mean(&h), expected_mean);
        Histogram_free(&h);
        return;
    }
    
    // variance = (1*(1.0-3.4)^2 + 2*(3.0-3.4)^2 + 2*(5.0-3.4)^2) / 5 = 2.24
    double expected_var = 2.24;
    if (fabs(variance(&h) - expected_var) > EPS) {
        printf("Test 5 FAILED: variance() = %f, expected = %f\n", variance(&h), expected_var);
        Histogram_free(&h);
        return;
    }
    
    // dev = (1*|1.0-3.4| + 2*|3.0-3.4| + 2*|5.0-3.4|) / 5 = 1.28
    double expected_dev = 1.28;
    if (fabs(dev(&h) - expected_dev) > EPS) {
        printf("Test 5 FAILED: dev() = %f, expected = %f\n", dev(&h), expected_dev);
        Histogram_free(&h);
        return;
    }
    
    // median: target = 2.5.
    // Накопичена частота: біні 0 -> 1, біні 1 -> 3. Медіана в біні 1 (межі [2, 4)).
    // median = 2.0 + 2.0 * ((2.5 - 1.0) / 2) = 3.5
    double expected_med = 3.5;
    if (fabs(median(&h) - expected_med) > EPS) {
        printf("Test 5 FAILED: median() = %f, expected = %f\n", median(&h), expected_med);
        Histogram_free(&h);
        return;
    }
    
    // skewness:
    // E[(X-mu)^3] = (1*(1.0-3.4)^3 + 2*(3.0-3.4)^3 + 2*(5.0-3.4)^3) / 5
    //             = (-13.824 + 2*(-0.064) + 2*(4.096)) / 5
    //             = (-13.824 - 0.128 + 8.192) / 5 = -5.76 / 5 = -1.152
    // skewness = -1.152 / (2.24^1.5) = -1.152 / 3.35106 = -0.34377
    double expected_skew = -0.343622;
    if (fabs(skewness(&h) - expected_skew) > EPS) {
        printf("Test 5 FAILED: skewness() = %f, expected = %f\n", skewness(&h), expected_skew);
        Histogram_free(&h);
        return;
    }
    
    // kurtosis:
    // E[(X-mu)^4] = (1*(-2.4)^4 + 2*(-0.4)^4 + 2*(1.6)^4) / 5
    //             = (33.1776 + 2*0.0256 + 2*6.5536) / 5
    //             = (33.1776 + 0.0512 + 13.1072) / 5 = 46.336 / 5 = 9.2672
    // kurtosis = 9.2672 / (2.24^2) - 3.0 = 9.2672 / 5.0176 - 3.0 = 1.8469 - 3.0 = -1.153
    double expected_kurt = -1.153;
    if (fabs(kurtosis(&h) - expected_kurt) > 1e-3) {
        printf("Test 5 FAILED: kurtosis() = %f, expected = %f\n", kurtosis(&h), expected_kurt);
        Histogram_free(&h);
        return;
    }
    
    // entropy:
    // p0 = 1/5 = 0.2, p1 = 2/5 = 0.4, p2 = 0.4
    // ent = -(0.2*ln(0.2) + 2 * 0.4*ln(0.4)) = -(0.2*(-1.6094) + 0.8*(-0.9163))
    //     = -(-0.3219 - 0.733) = 1.0549
    double expected_ent = 1.0549;
    if (fabs(entropy(&h) - expected_ent) > 1e-3) {
        printf("Test 5 FAILED: entropy() = %f, expected = %f\n", entropy(&h), expected_ent);
        Histogram_free(&h);
        return;
    }
    
    Histogram_free(&h);
    printf("Test 5 (Statistics) passed\n");
}

// Тест 6: Робота з файлами
void test_file_io(void) {
    Histogram h1, h2;
    Histogram_init(&h1, 5, -5.0, 5.0);
    double vals[10] = {-4.0, -3.0, -1.0, 0.0, 1.0, 1.5, 2.0, 3.0, 4.0, 4.5};
    addBatch(&h1, vals, 10, 1);
    
    // Збереження та завантаження тексту
    if (!saveToTextFile(&h1, "test_hist.txt")) {
        printf("Test 6 FAILED: saveToTextFile returned error.\n");
        Histogram_free(&h1);
        return;
    }
    
    Histogram_init(&h2, 1, 0, 1);
    if (!loadFromTextFile(&h2, "test_hist.txt")) {
        printf("Test 6 FAILED: loadFromTextFile returned error.\n");
        Histogram_free(&h1);
        Histogram_free(&h2);
        return;
    }
    
    if (h1.M != h2.M || h1.min_hist != h2.min_hist || h1.max_hist != h2.max_hist || h1.total != h2.total) {
        printf("Test 6 FAILED: Loaded text histogram fields differ.\n");
        Histogram_free(&h1);
        Histogram_free(&h2);
        return;
    }
    
    for (unsigned i = 0; i < h1.M; i++) {
        if (h1.frequency[i] != h2.frequency[i]) {
            printf("Test 6 FAILED: Loaded text frequencies differ.\n");
            Histogram_free(&h1);
            Histogram_free(&h2);
            return;
        }
    }
    Histogram_free(&h2);
    
    // Збереження та завантаження бінарного файлу
    if (!saveToBinaryFile(&h1, "test_hist.bin")) {
        printf("Test 6 FAILED: saveToBinaryFile returned error.\n");
        Histogram_free(&h1);
        return;
    }
    
    Histogram_init(&h2, 1, 0, 1);
    if (!loadFromBinaryFile(&h2, "test_hist.bin")) {
        printf("Test 6 FAILED: loadFromBinaryFile returned error.\n");
        Histogram_free(&h1);
        Histogram_free(&h2);
        return;
    }
    
    if (h1.M != h2.M || h1.min_hist != h2.min_hist || h1.max_hist != h2.max_hist || h1.total != h2.total) {
        printf("Test 6 FAILED: Loaded binary histogram fields differ.\n");
        Histogram_free(&h1);
        Histogram_free(&h2);
        return;
    }
    
    for (unsigned i = 0; i < h1.M; i++) {
        if (h1.frequency[i] != h2.frequency[i]) {
            printf("Test 6 FAILED: Loaded binary frequencies differ.\n");
            Histogram_free(&h1);
            Histogram_free(&h2);
            return;
        }
    }
    
    Histogram_free(&h1);
    Histogram_free(&h2);
    
    // Видаляємо тимчасові файли
    remove("test_hist.txt");
    remove("test_hist.bin");
    
    printf("Test 6 (File I/O) passed\n");
}

// Тест 7: Зчитування з рядка
void test_input_from_string(void) {
    Histogram h;
    Histogram_init(&h, 5, 0.0, 10.0);
    const char* str = " 1.5, 2.5; 3.5  8.5   12.0"; // 12.0 поза межами (буде проігноровано в clip_mode=1)
    int chars = inputFromString(&h, str);
    
    if (chars == 0) {
        printf("Test 7 FAILED: inputFromString returned 0 characters.\n");
        Histogram_free(&h);
        return;
    }
    
    // Очікуємо, що зчитано 4 правильних числа в межах (1.5, 2.5, 3.5, 8.5) та одне проігнороване (12.0)
    // Всього total має бути 4
    if (h.total != 4) {
        printf("Test 7 FAILED: inputFromString total count is %lu (expected 4).\n", h.total);
        Histogram_free(&h);
        return;
    }
    
    Histogram_free(&h);
    printf("Test 7 (Input from String) passed\n");
}

// Тест 8: Перевірка гіпотез
void test_hypotheses(void) {
    Histogram h;
    // 1. Рівномірний розподіл
    Histogram_init(&h, 5, 0.0, 5.0);
    // Додамо однакову кількість у кожен бін
    for (int i = 0; i < 20; i++) {
        addNumber(&h, 0.5, 1);
        addNumber(&h, 1.5, 1);
        addNumber(&h, 2.5, 1);
        addNumber(&h, 3.5, 1);
        addNumber(&h, 4.5, 1);
    }
    
    int unif_res = testUniform(&h);
    if (unif_res != 1) {
        printf("Test 8 FAILED: testUniform rejected perfectly uniform data.\n");
        Histogram_free(&h);
        return;
    }
    
    // 2. Експоненційний розподіл
    Histogram_free(&h);
    Histogram_init(&h, 5, 0.0, 5.0);
    // Згенеруємо приблизно експоненційний розподіл (f(x) = exp(-x))
    // e^0 = 1, e^-1 = 0.36, e^-2 = 0.13, e^-3 = 0.05, e^-4 = 0.02
    // Додамо відповідні пропорції
    for (int i = 0; i < 100; i++) addNumber(&h, 0.5, 1);
    for (int i = 0; i < 37;  i++) addNumber(&h, 1.5, 1);
    for (int i = 0; i < 14;  i++) addNumber(&h, 2.5, 1);
    for (int i = 0; i < 5;   i++) addNumber(&h, 3.5, 1);
    for (int i = 0; i < 2;   i++) addNumber(&h, 4.5, 1);
    
    int exp_res = testExponential(&h);
    // Тут просто перевіряємо, що функція працює і повертає 0 або 1
    if (exp_res != 0 && exp_res != 1) {
        printf("Test 8 FAILED: testExponential returned invalid code %d.\n", exp_res);
        Histogram_free(&h);
        return;
    }
    
    // 3. Нормальний розподіл
    Histogram_free(&h);
    Histogram_init(&h, 5, -2.5, 2.5);
    // Біноміальне наближення до нормального: біни серединами: -2, -1, 0, 1, 2
    // Пропорції: 1, 4, 6, 4, 1
    for (int i = 0; i < 5;   i++) addNumber(&h, -2.0, 1);
    for (int i = 0; i < 20;  i++) addNumber(&h, -1.0, 1);
    for (int i = 0; i < 30;  i++) addNumber(&h, 0.0, 1);
    for (int i = 0; i < 20;  i++) addNumber(&h, 1.0, 1);
    for (int i = 0; i < 5;   i++) addNumber(&h, 2.0, 1);
    
    int norm_res = testNormal(&h);
    if (norm_res != 0 && norm_res != 1) {
        printf("Test 8 FAILED: testNormal returned invalid code %d.\n", norm_res);
        Histogram_free(&h);
        return;
    }
    
    // 4. Біноміальний розподіл
    int bin_res = testBinomial(&h, 4, 0.5);
    if (bin_res != 0 && bin_res != 1) {
        printf("Test 8 FAILED: testBinomial returned invalid code %d.\n", bin_res);
        Histogram_free(&h);
        return;
    }
    
    // 5. Розподіл Пуассона
    int pois_res = testPoisson(&h, 1.0);
    if (pois_res != 0 && pois_res != 1) {
        printf("Test 8 FAILED: testPoisson returned invalid code %d.\n", pois_res);
        Histogram_free(&h);
        return;
    }
    
    Histogram_free(&h);
    printf("Test 8 (Hypotheses Testing) passed\n");
}

void run_all_tests(void) {
    printf("--- Запуск модульних тестів (C-версія) ---\n");
    test_init_free();
    test_setters();
    test_add_number();
    test_add_batch();
    test_statistics();
    test_file_io();
    test_input_from_string();
    test_hypotheses();
    printf("--- Всі юніт-тести завершено ---\n\n");
}

/* ========================================================================= *
 *                              ГОЛОВНА ПРОГРАМА                             *
 * ========================================================================= */

int main(void) {
    // 1. Запуск тестів
    run_all_tests();
    
    // 2. Користувацький режим
    printf("Бажаєте запустити демонстраційний режим? (y/n): ");
    char choice_demo;
    if (scanf(" %c", &choice_demo) != 1 || (choice_demo != 'y' && choice_demo != 'Y')) {
        printf("Вихід з програми.\n");
        return 0;
    }
    
    printf("\nОберіть режим введення даних:\n");
    printf("1 - Консольне введення чисел вручну\n");
    printf("2 - Зчитування з текстового файлу (Histogram_test.dat)\n");
    printf("3 - Зчитування з бінарного файлу\n");
    printf("Вибір: ");
    int mode;
    if (scanf("%d", &mode) != 1) {
        printf("Некоректний вибір. Завершення.\n");
        return 1;
    }
    
    Histogram h;
    // Оптимальні параметри за замовчуванням
    unsigned M = 10;
    double min_v = 0.0;
    double max_v = 100.0;
    
    printf("Введіть кількість стовпців (M) [default: 10]: ");
    unsigned temp_M;
    if (scanf("%u", &temp_M) == 1 && temp_M > 0) M = temp_M;
    
    printf("Введіть мінімальну межу гістограми [default: 0.0]: ");
    double temp_min;
    if (scanf("%lf", &temp_min) == 1) min_v = temp_min;
    
    printf("Введіть максимальну межу гістограми [default: 100.0]: ");
    double temp_max;
    if (scanf("%lf", &temp_max) == 1 && temp_max > min_v) max_v = temp_max;
    
    if (Histogram_init(&h, M, min_v, max_v) != 0) {
        printf("Помилка ініціалізації гістограми.\n");
        return 1;
    }
    
    printf("Оберіть режим виходу за межі діапазону (clip_mode):\n");
    printf("1 - Ігнорувати числа\n");
    printf("2 - Додавати до крайніх стовпців\n");
    printf("Вибір: ");
    int clip_mode;
    if (scanf("%d", &clip_mode) != 1 || (clip_mode != 1 && clip_mode != 2)) {
        clip_mode = 1;
        printf("Вибрано режим за замовчуванням (1 - ігнорувати).\n");
    }
    
    if (mode == 1) {
        printf("\nВводьте числа (розділені пробілом або новим рядком).\n");
        printf("Для закінчення введення введіть будь-який символ, що не є числом (наприклад, 'q'):\n");
        double val;
        int count = 0;
        while (scanf("%lf", &val) == 1) {
            addNumber(&h, val, clip_mode);
            count++;
        }
        // Очистити stdin
        char dummy;
        while (scanf("%c", &dummy) == 1 && dummy != '\n');
        printf("Успішно введено чисел з консолі: %d\n", count);
    } 
    else if (mode == 2) {
        // Зчитування з Histogram_test.dat
        int res = addFromTextFile(&h, "Histogram_test.dat", clip_mode);
        if (res < 0) {
            // Спробуємо створити файл за замовчуванням, якщо його немає
            printf("Файл Histogram_test.dat не знайдено. Створюємо автоматично з 20+ числами...\n");
            FILE* df = fopen("Histogram_test.dat", "w");
            if (df) {
                // Запишемо 25 чисел у межах 0..100 (суміш різних значень)
                double test_vals[25] = {
                    10.5, 15.2, 23.4, 25.0, 31.8, 33.3, 45.0, 47.9, 49.1, 52.0,
                    55.5, 58.0, 62.1, 64.0, 66.8, 70.2, 75.0, 77.3, 81.0, 85.5,
                    90.0, 92.4, 95.0, 98.1, 102.5
                };
                for (int i = 0; i < 25; i++) {
                    fprintf(df, "%f\n", test_vals[i]);
                }
                fclose(df);
                res = addFromTextFile(&h, "Histogram_test.dat", clip_mode);
            }
        }
        if (res >= 0) {
            printf("Успішно зчитано з текстового файлу: %d чисел.\n", res);
        } else {
            printf("Не вдалося відкрити або зчитати файл Histogram_test.dat\n");
            Histogram_free(&h);
            return 1;
        }
    } 
    else if (mode == 3) {
        printf("Введіть шлях до бінарного файлу: ");
        char bin_path[256];
        if (scanf("%255s", bin_path) != 1) {
            printf("Некоректний шлях.\n");
            Histogram_free(&h);
            return 1;
        }
        int res = addFromBinaryFile(&h, bin_path, clip_mode);
        if (res >= 0) {
            printf("Успішно зчитано з бінарного файлу: %d чисел.\n", res);
        } else {
            printf("Не вдалося відкрити або зчитати бінарний файл %s\n", bin_path);
            Histogram_free(&h);
            return 1;
        }
    } 
    else {
        printf("Невідомий режим.\n");
        Histogram_free(&h);
        return 1;
    }
    
    // 3. Розрахунок показників та виведення
    printHistogram(&h);
    
    double m = mean(&h);
    double med = median(&h);
    double d = dev(&h);
    double v = variance(&h);
    double s = skewness(&h);
    double k = kurtosis(&h);
    double e = entropy(&h);
    
    printf("--- Статистичні характеристики ---\n");
    printf("Кількість елементів:  %lu\n", num(&h));
    printf("Середнє значення (mean): %f\n", m);
    printf("Медіана (median):       %f\n", med);
    printf("Середнє відхилення:      %f\n", d);
    printf("Дисперсія (variance):   %f\n", v);
    printf("Асиметрія (skewness):   %f\n", s);
    printf("Ексцес (kurtosis):      %f\n", k);
    printf("Ентропія Шеннона:       %f\n", e);
    printf("\n");
    
    printf("--- Результати перевірки статистичних гіпотез (chi2, alpha = 0.05) ---\n");
    printf("Рівномірний розподіл:     %s\n", testUniform(&h) ? "ПРИЙНЯТО" : "ВІДХИЛЕНО");
    printf("Нормальний розподіл:      %s\n", testNormal(&h) ? "ПРИЙНЯТО" : "ВІДХИЛЕНО");
    printf("Експоненційний розподіл:  %s\n", testExponential(&h) ? "ПРИЙНЯТО" : "ВІДХИЛЕНО");
    printf("Біноміальний (trials=%u, p=0.5): %s\n", M, testBinomial(&h, M, 0.5) ? "ПРИЙНЯТО" : "ВІДХИЛЕНО");
    printf("Пуассона (lambda=5.0):    %s\n", testPoisson(&h, 5.0) ? "ПРИЙНЯТО" : "ВІДХИЛЕНО");
    printf("\n");
    
    // Запис результатів у файли
    printf("Зберігаємо результати у файли...\n");
    saveToTextFile(&h, "histogram_saved.txt");
    saveToBinaryFile(&h, "histogram_saved.bin");
    
    // Створимо звіт у текстовому форматі
    FILE* rf = fopen("histogram_report.txt", "w");
    if (rf) {
        fprintf(rf, "=== ЗВІТ ПРО СТАТИСТИЧНУ ОБРОБКУ ДАНИХ ===\n\n");
        fprintf(rf, "Гістограма: діапазон [%f, %f], стовпців: %u, режим clip_mode: %d\n", h.min_hist, h.max_hist, h.M, clip_mode);
        fprintf(rf, "Загальна кількість чисел: %lu\n\n", num(&h));
        
        fprintf(rf, "--- Статистичні характеристики ---\n");
        fprintf(rf, "Середнє значення (mean): %f\n", m);
        fprintf(rf, "Медіана (median):       %f\n", med);
        fprintf(rf, "Середнє відхилення:      %f\n", d);
        fprintf(rf, "Дисперсія (variance):   %f\n", v);
        fprintf(rf, "Асиметрія (skewness):   %f\n", s);
        fprintf(rf, "Ексцес (kurtosis):      %f\n", k);
        fprintf(rf, "Ентропія Шеннона:       %f\n\n", e);
        
        fprintf(rf, "--- Перевірка статистичних гіпотез (рівень 0.05) ---\n");
        fprintf(rf, "Рівномірний розподіл:     %s\n", testUniform(&h) ? "ПРИЙНЯТО" : "ВІДХИЛЕНО");
        fprintf(rf, "Нормальний розподіл:      %s\n", testNormal(&h) ? "ПРИЙНЯТО" : "ВІДХИЛЕНО");
        fprintf(rf, "Експоненційний розподіл:  %s\n", testExponential(&h) ? "ПРИЙНЯТО" : "ВІДХИЛЕНО");
        fclose(rf);
        printf("Звіт збережено у файл 'histogram_report.txt'.\n");
    }
    
    Histogram_free(&h);
    return 0;
}
