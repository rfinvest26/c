/*
 * File: Histogram.h
 * Task: Histogram — структура для статистичної обробки даних (завдання 13)
 * Group: К-24
 * Author: Курлов Микита
 * Date: 2026-05-19
 * Description: Заголовочний файл класу C++ Histogram (з коментарями Doxygen).
 *              Клас загорнутий в простір імен cpp для запобігання
 *              конфліктів імен при крос-тестуванні з C-кодом.
 */

#ifndef HISTOGRAM_CPP_H
#define HISTOGRAM_CPP_H

#include <cstddef>

namespace cpp {

/**
 * @brief Клас для статистичної обробки даних у вигляді гістограми частот.
 */
class Histogram {
private:
    double min_hist;      /**< Мінімальне значення діапазону */
    double max_hist;      /**< Максимальне значення діапазону */
    unsigned M;           /**< Кількість стовпців (бінів) гістограми */
    unsigned* frequency;  /**< Динамічний масив частот для кожного біну */
    unsigned long total;  /**< Загальна кількість доданих елементів */

public:
    /**
     * @brief Конструктор за замовчуванням та з параметрами.
     * @param[in] M - кількість стовпців гістограми (за замовчуванням 10)
     * @param[in] min - мінімальна межа діапазону (за замовчуванням 0.0)
     * @param[in] max - максимальна межа діапазону (за замовчуванням 100.0)
     */
    Histogram(unsigned M = 10, double min = 0.0, double max = 100.0);

    /**
     * @brief Деструктор класу. Звільняє виділену пам'ять.
     */
    ~Histogram();

private:
    /**
     * @brief Конструктор копіювання (заборонений для безпеки роботи з динамічною пам'яттю).
     */
    Histogram(const Histogram& other);

    /**
     * @brief Оператор присвоювання (заборонений).
     */
    Histogram& operator=(const Histogram& other);

public:

    /**
     * @brief Встановлює нове максимальне значення діапазону.
     * @param[in] m - нове максимальне значення
     */
    void setMax(double m);

    /**
     * @brief Встановлює нове мінімальне значення діапазону.
     * @param[in] m - нове мінімальне значення
     */
    void setMin(double m);

    /**
     * @brief Встановлює нову кількість стовпців гістограми з перерозподілом пам'яті.
     * @param[in] m - нова кількість стовпців
     */
    void setM(unsigned m);

    /**
     * @brief Додає одне число до гістограми.
     * @param[in] x - число для додавання
     * @param[in] clip_mode - режим обробки виходу за межі (1 - ігнорувати, 2 - до крайніх бінів)
     */
    void addNumber(double x, int clip_mode);

    /**
     * @brief Додає масив чисел до гістограми.
     * @param[in] data - масив чисел
     * @param[in] dataSize - розмір масиву
     * @param[in] clip_mode - режим обробки виходу за межі (1 або 2)
     */
    void addBatch(double* data, std::size_t dataSize, int clip_mode);

    /**
     * @brief Зчитує та додає числа з текстового файлу.
     * @param[in] filename - ім'я текстового файлу
     * @param[in] clip_mode - режим обробки виходу за межі (1 або 2)
     * @return Кількість успішно доданих чисел, або -1 при помилці
     */
    int addFromTextFile(const char* filename, int clip_mode);

    /**
     * @brief Зчитує та додає числа з бінарного файлу.
     * @param[in] filename - ім'я бінарного файлу
     * @param[in] clip_mode - режим обробки виходу за межі (1 або 2)
     * @return Кількість успішно доданих чисел, або -1 при помилці
     */
    int addFromBinaryFile(const char* filename, int clip_mode);

    /**
     * @brief Повертає загальну кількість елементів у гістограмі.
     * @return Загальна кількість доданих елементів
     */
    unsigned long num() const;

    /**
     * @brief Повертає частоту стовпця з індексом i.
     * @param[in] i - індекс стовпця (від 0 до M-1)
     * @return Частота стовпця i
     */
    unsigned numHist(unsigned i) const;

    /**
     * @brief Обчислює середнє значення на основі гістограми.
     * @return Оцінка середнього значення
     */
    double mean() const;

    /**
     * @brief Обчислює медіану на основі гістограми.
     * @return Оцінка медіани
     */
    double median() const;

    /**
     * @brief Обчислює середнє абсолютне відхилення.
     * @return Оцінка середнього відхилення
     */
    double dev() const;

    /**
     * @brief Обчислює дисперсію на основі гістограми.
     * @return Оцінка дисперсії
     */
    double variance() const;

    /**
     * @brief Обчислює асиметрію (skewness) на основі гістограми.
     * @return Оцінка коефіцієнта асиметрії
     */
    double skewness() const;

    /**
     * @brief Обчислює ексцес (kurtosis) на основі гістограми.
     * @return Оцінка коефіцієнта ексцесу (excess kurtosis)
     */
    double kurtosis() const;

    /**
     * @brief Обчислює інформаційну ентропію Шеннона для гістограми.
     * @return Ентропія в натах (при використанні натурального логарифма)
     */
    double entropy() const;

    /**
     * @brief Перевіряє гіпотезу про нормальний розподіл (критерій Пірсона χ²).
     * @return 1 якщо гіпотеза не відхиляється на рівні значущості 0.05, 0 — якщо відхиляється
     */
    int testNormal() const;

    /**
     * @brief Перевіряє гіпотезу про експоненційний розподіл (критерій Пірсона χ²).
     * @return 1 якщо гіпотеза не відхиляється на рівні значущості 0.05, 0 — якщо відхиляється
     */
    int testExponential() const;

    /**
     * @brief Перевіряє гіпотезу про рівномірний розподіл (критерій Пірсона χ²).
     * @return 1 якщо гіпотеза не відхиляється на рівні значущості 0.05, 0 — якщо відхиляється
     */
    int testUniform() const;

    /**
     * @brief Перевіряє гіпотезу про біноміальний розподіл (критерій Пірсона χ²).
     * @param[in] n_trials - кількість випробувань біноміального розподілу
     * @param[in] p - ймовірність успіху в одному випробуванні
     * @return 1 якщо гіпотеза не відхиляється на рівні значущості 0.05, 0 — якщо відхиляється
     */
    int testBinomial(unsigned n_trials, double p) const;

    /**
     * @brief Перевіряє гіпотезу про розподіл Пуассона (критерій Пірсона χ²).
     * @param[in] lambda - інтенсивність (параметр λ) розподілу Пуассона
     * @return 1 якщо гіпотеза не відхиляється на рівні значущості 0.05, 0 — якщо відхиляється
     */
    int testPoisson(double lambda) const;

    /**
     * @brief Виводить гістограму у вигляді псевдографіки на консоль.
     */
    void printHistogram() const;

    /**
     * @brief Зберігає параметри та стан гістограми у текстовий файл.
     * @param[in] filename - ім'я текстового файлу
     * @return 1 при успішному збереженні, 0 при помилці
     */
    int saveToTextFile(const char* filename) const;

    /**
     * @brief Завантажує параметри та стан гістограми з текстового файлу.
     * @param[in] filename - ім'я текстового файлу
     * @return 1 при успішному завантаженні, 0 при помилці
     */
    int loadFromTextFile(const char* filename);

    /**
     * @brief Зберігає параметри та стан гістограми у бінарний файл.
     * @param[in] filename - ім'я бінарного файлу
     * @return 1 при успішному збереженні, 0 при помилці
     */
    int saveToBinaryFile(const char* filename) const;

    /**
     * @brief Завантажує параметри та стан гістограми з бінарного файлу.
     * @param[in] filename - ім'я бінарного файлу
     * @return 1 при успішному завантаженні, 0 при помилці
     */
    int loadFromBinaryFile(const char* filename);

    /**
     * @brief Зчитує числа з рядка символів та додає їх до гістограми.
     * @param[in] str - вхідний рядок символів
     * @return Кількість успішно оброблених символів рядка
     */
    int inputFromString(const char* str);
};

} // namespace cpp

#endif /* HISTOGRAM_CPP_H */
