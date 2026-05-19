/*
 * File: Histogram.h
 * Task: Histogram — структура для статистичної обробки даних (завдання 13)
 * Group: К-24
 * Author: Курлов нікіта
 * Date: 2026-05-19
 * Description: Заголовочний файл, що містить визначення структури Histogram
 *              та оголошення функцій для роботи з нею (з коментарями Doxygen).
 */

#ifndef HISTOGRAM_C_H
#define HISTOGRAM_C_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Структура для статистичної обробки даних у вигляді гістограми частот.
 */
typedef struct {
    double min_hist;      /**< Мінімальне значення діапазону */
    double max_hist;      /**< Максимальне значення діапазону */
    unsigned M;           /**< Кількість стовпців (бінів) гістограми */
    unsigned* frequency;  /**< Динамічний масив частот для кожного біну */
    unsigned long total;  /**< Загальна кількість доданих елементів */
} Histogram;

/**
 * @brief Ініціалізує структуру гістограми.
 * @param[out] h - вказівник на структуру гістограми
 * @param[in] M - кількість стовпців гістограми
 * @param[in] min - мінімальне значення діапазону
 * @param[in] max - максимальне значення діапазону
 * @return 0 при успішній ініціалізації, ненульове значення при помилці
 */
int Histogram_init(Histogram* h, unsigned M, double min, double max);

/**
 * @brief Звільняє динамічну пам'ять, виділену під гістограму.
 * @param[in,out] h - вказівник на структуру гістограми
 */
void Histogram_free(Histogram* h);

/**
 * @brief Встановлює нове максимальне значення діапазону.
 * @param[in,out] h - вказівник на структуру гістограми
 * @param[in] m - нове максимальне значення
 */
void setMax(Histogram* h, double m);

/**
 * @brief Встановлює нове мінімальне значення діапазону.
 * @param[in,out] h - вказівник на структуру гістограми
 * @param[in] m - нове мінімальне значення
 */
void setMin(Histogram* h, double m);

/**
 * @brief Встановлює нову кількість стовпців гістограми з перерозподілом пам'яті.
 * @param[in,out] h - вказівник на структуру гістограми
 * @param[in] m - нова кількість стовпців
 */
void setM(Histogram* h, unsigned m);

/**
 * @brief Додає одне число до гістограми.
 * @param[in,out] h - вказівник на структуру гістограми
 * @param[in] x - число для додавання
 * @param[in] clip_mode - режим обробки виходу за межі:
 *                        1 - ігнорувати числа поза діапазоном,
 *                        2 - додавати до крайніх стовпців
 */
void addNumber(Histogram* h, double x, int clip_mode);

/**
 * @brief Додає масив чисел до гістограми.
 * @param[in,out] h - вказівник на структуру гістограми
 * @param[in] data - вказівник на масив чисел
 * @param[in] dataSize - розмір масиву
 * @param[in] clip_mode - режим обробки виходу за межі (1 або 2)
 */
void addBatch(Histogram* h, double* data, size_t dataSize, int clip_mode);

/**
 * @brief Зчитує та додає числа з текстового файлу.
 * @param[in,out] h - вказівник на структуру гістограми
 * @param[in] filename - ім'я текстового файлу
 * @param[in] clip_mode - режим обробки виходу за межі (1 або 2)
 * @return Кількість успішно доданих чисел, або -1 при помилці відкриття файлу
 */
int addFromTextFile(Histogram* h, const char* filename, int clip_mode);

/**
 * @brief Зчитує та додає числа з бінарного файлу.
 * @param[in,out] h - вказівник на структуру гістограми
 * @param[in] filename - ім'я бінарного файлу
 * @param[in] clip_mode - режим обробки виходу за межі (1 або 2)
 * @return Кількість успішно доданих чисел, або -1 при помилці відкриття файлу
 */
int addFromBinaryFile(Histogram* h, const char* filename, int clip_mode);

/**
 * @brief Повертає загальну кількість елементів у гістограмі.
 * @param[in] h - вказівник на структуру гістограми
 * @return Загальна кількість доданих елементів
 */
unsigned long num(const Histogram* h);

/**
 * @brief Повертає частоту стовпця з індексом i.
 * @param[in] h - вказівник на структуру гістограми
 * @param[in] i - індекс стовпця (від 0 до M-1)
 * @return Частота стовпця i
 */
unsigned numHist(const Histogram* h, unsigned i);

/**
 * @brief Обчислює середнє значення на основі гістограми.
 * @param[in] h - вказівник на структуру гістограми
 * @return Оцінка середнього значення
 */
double mean(const Histogram* h);

/**
 * @brief Обчислює медіану на основі гістограми.
 * @param[in] h - вказівник на структуру гістограми
 * @return Оцінка медіани
 */
double median(const Histogram* h);

/**
 * @brief Обчислює середнє абсолютне відхилення.
 * @param[in] h - вказівник на структуру гістограми
 * @return Оцінка середнього відхилення
 */
double dev(const Histogram* h);

/**
 * @brief Обчислює дисперсію на основі гістограми.
 * @param[in] h - вказівник на структуру гістограми
 * @return Оцінка дисперсії
 */
double variance(const Histogram* h);

/**
 * @brief Обчислює асиметрію (skewness) на основі гістограми.
 * @param[in] h - вказівник на структуру гістограми
 * @return Оцінка коефіцієнта асиметрії
 */
double skewness(const Histogram* h);

/**
 * @brief Обчислює ексцес (kurtosis) на основі гістограми.
 * @param[in] h - вказівник на структуру гістограми
 * @return Оцінка коефіцієнта ексцесу (excess kurtosis)
 */
double kurtosis(const Histogram* h);

/**
 * @brief Обчислює інформаційну ентропію Шеннона для гістограми.
 * @param[in] h - вказівник на структуру гістограми
 * @return Ентропія в натах (при використанні натурального логарифма)
 */
double entropy(const Histogram* h);

/**
 * @brief Перевіряє гіпотезу про нормальний розподіл (критерій Пірсона χ²).
 * @param[in] h - вказівник на структуру гістограми
 * @return 1 якщо гіпотеза не відхиляється на рівні значущості 0.05, 0 — якщо відхиляється
 */
int testNormal(const Histogram* h);

/**
 * @brief Перевіряє гіпотезу про експоненційний розподіл (критерій Пірсона χ²).
 * @param[in] h - вказівник на структуру гістограми
 * @return 1 якщо гіпотеза не відхиляється на рівні значущості 0.05, 0 — якщо відхиляється
 */
int testExponential(const Histogram* h);

/**
 * @brief Перевіряє гіпотезу про рівномірний розподіл (критерій Пірсона χ²).
 * @param[in] h - вказівник на структуру гістограми
 * @return 1 якщо гіпотеза не відхиляється на рівні значущості 0.05, 0 — якщо відхиляється
 */
int testUniform(const Histogram* h);

/**
 * @brief Перевіряє гіпотезу про біноміальний розподіл (критерій Пірсона χ²).
 * @param[in] h - вказівник на структуру гістограми
 * @param[in] n_trials - кількість випробувань біноміального розподілу
 * @param[in] p - ймовірність успіху в одному випробуванні
 * @return 1 якщо гіпотеза не відхиляється на рівні значущості 0.05, 0 — якщо відхиляється
 */
int testBinomial(const Histogram* h, unsigned n_trials, double p);

/**
 * @brief Перевіряє гіпотезу про розподіл Пуассона (критерій Пірсона χ²).
 * @param[in] h - вказівник на структуру гістограми
 * @param[in] lambda - інтенсивність (параметр λ) розподілу Пуассона
 * @return 1 якщо гіпотеза не відхиляється на рівні значущості 0.05, 0 — якщо відхиляється
 */
int testPoisson(const Histogram* h, double lambda);

/**
 * @brief Виводить гістограму у вигляді псевдографіки на консоль.
 * @param[in] h - вказівник на структуру гістограми
 */
void printHistogram(const Histogram* h);

/**
 * @brief Зберігає параметри та стан гістограми у текстовий файл.
 * @param[in] h - вказівник на структуру гістограми
 * @param[in] filename - ім'я текстового файлу
 * @return 1 при успішному збереженні, 0 при помилці
 */
int saveToTextFile(const Histogram* h, const char* filename);

/**
 * @brief Завантажує параметри та стан гістограми з текстового файлу.
 * @param[in,out] h - вказівник на структуру гістограми
 * @param[in] filename - ім'я текстового файлу
 * @return 1 при успішному завантаженні, 0 при помилці
 */
int loadFromTextFile(Histogram* h, const char* filename);

/**
 * @brief Зберігає параметри та стан гістограми у бінарний файл.
 * @param[in] h - вказівник на структуру гістограми
 * @param[in] filename - ім'я бінарного файлу
 * @return 1 при успішному збереженні, 0 при помилці
 */
int saveToBinaryFile(const Histogram* h, const char* filename);

/**
 * @brief Завантажує параметри та стан гістограми з бінарного файлу.
 * @param[in,out] h - вказівник на структуру гістограми
 * @param[in] filename - ім'я бінарного файлу
 * @return 1 при успішному завантаженні, 0 при помилці
 */
int loadFromBinaryFile(Histogram* h, const char* filename);

/**
 * @brief Зчитує числа з рядка символів та додає їх до гістограми.
 * @param[in,out] h - вказівник на структуру гістограми
 * @param[in] str - вхідний рядок символів
 * @return Кількість успішно оброблених символів рядка
 */
int inputFromString(Histogram* h, const char* str);

#ifdef __cplusplus
}
#endif

#endif /* HISTOGRAM_C_H */
