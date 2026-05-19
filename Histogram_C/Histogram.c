/*
 * File: Histogram.c
 * Task: Histogram — структура для статистичної обробки даних (завдання 13)
 * Group: К-24
 * Author: Курлов Микита
 * Date: 2026-05-19
 * Description: Реалізація всіх функцій для роботи з гістограмою
 *              (ініціалізація, зміна налаштувань, додавання даних,
 *              обчислення статистичних характеристик, перевірка гіпотез,
 *              введення-виведення).
 */

#include "Histogram.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

/* ========================================================================= *
 *                       ВНУТРІШНІ ДОПОМІЖНІ ФУНКЦІЇ                         *
 * ========================================================================= */

/**
 * @brief Ланцош наближення для обчислення натурального логарифма гамма-функції ln(Г(x)).
 */
static double log_gamma(double xx) {
    double x = xx - 1.0;
    double tmp = x + 5.5;
    tmp -= (x + 0.5) * log(tmp);
    double ser = 1.000000000190015;
    static const double cof[6] = {
        76.18009172947146, -86.50532032941677,
        24.01409824083091, -1.231739572450155,
        0.1208650973866179e-2, -0.5395239384953e-5
    };
    for (int j = 0; j < 6; j++) {
        x += 1.0;
        ser += cof[j] / x;
    }
    return -tmp + log(2.5066282746310005 * ser);
}

/**
 * @brief Допоміжна функція для обчислення неповної гамма-функції через ряд P(a, x).
 */
static double gser(double a, double x) {
    if (x <= 0.0) return 0.0;
    double sum = 1.0 / a;
    double del = sum;
    double ap = a;
    for (int n = 1; n <= 100; n++) {
        ap += 1.0;
        del *= x / ap;
        sum += del;
        if (fabs(del) < fabs(sum) * 3e-7) break;
    }
    return sum * exp(-x + a * log(x) - log_gamma(a));
}

/**
 * @brief Допоміжна функція для обчислення неповної гамма-функції через ланцюговий дріб Q(a, x).
 */
static double gcf(double a, double x) {
    double b = x + 1.0 - a;
    double c = 1.0 / 1e-30;
    double d = 1.0 / b;
    double h = d;
    for (int i = 1; i <= 100; i++) {
        double an = -i * (i - a);
        b += 2.0;
        d = an * d + b;
        if (fabs(d) < 1e-30) d = 1e-30;
        c = b + an / c;
        if (fabs(c) < 1e-30) c = 1e-30;
        d = 1.0 / d;
        double del = d * c;
        h *= del;
        if (fabs(del - 1.0) < 3e-7) break;
    }
    return h * exp(-x + a * log(x) - log_gamma(a));
}

/**
 * @brief Обчислює регулярну неповну гамма-функцію Q(a, x) = 1 - P(a, x).
 */
static double gammq(double a, double x) {
    if (x < 0.0 || a <= 0.0) return 0.0;
    if (x < a + 1.0) {
        return 1.0 - gser(a, x);
    } else {
        return gcf(a, x);
    }
}

/**
 * @brief Функція стандартного нормального інтеграла ймовірностей Ф(z).
 */
static double phi_std(double z) {
    return 0.5 * (1.0 + erf(z / sqrt(2.0)));
}

/**
 * @brief Обчислює ймовірність для біноміального PMF P(X=k).
 */
static double binom_pmf(unsigned k, unsigned n, double p) {
    if (k > n || p < 0.0 || p > 1.0) return 0.0;
    if (p == 0.0) return (k == 0) ? 1.0 : 0.0;
    if (p == 1.0) return (k == n) ? 1.0 : 0.0;
    double log_comb = log_gamma(n + 1.0) - log_gamma(k + 1.0) - log_gamma(n - k + 1.0);
    double log_prob = log_comb + k * log(p) + (n - k) * log(1.0 - p);
    return exp(log_prob);
}

/**
 * @brief Обчислює ймовірність для PMF розподілу Пуассона P(X=k).
 */
static double poisson_pmf(unsigned k, double lambda) {
    if (lambda <= 0.0) return 0.0;
    double log_prob = (double)k * log(lambda) - lambda - log_gamma((double)k + 1.0);
    return exp(log_prob);
}

/* ========================================================================= *
 *                            РЕАЛІЗАЦІЯ ФУНКЦІЙ                             *
 * ========================================================================= */

int Histogram_init(Histogram* h, unsigned M, double min, double max) {
    if (!h || M == 0 || min >= max) {
        return 1;
    }
    h->min_hist = min;
    h->max_hist = max;
    h->M = M;
    h->total = 0;
    h->frequency = (unsigned*)calloc(M, sizeof(unsigned));
    if (!h->frequency) {
        return 1;
    }
    return 0;
}

void Histogram_free(Histogram* h) {
    if (h) {
        free(h->frequency);
        h->frequency = NULL;
        h->total = 0;
        h->M = 0;
    }
}

void setMax(Histogram* h, double m) {
    if (h && m > h->min_hist) {
        h->max_hist = m;
    }
}

void setMin(Histogram* h, double m) {
    if (h && m < h->max_hist) {
        h->min_hist = m;
    }
}

void setM(Histogram* h, unsigned m) {
    if (!h || m == 0) return;
    if (h->M != m) {
        unsigned* new_freq = (unsigned*)realloc(h->frequency, m * sizeof(unsigned));
        if (new_freq) {
            h->frequency = new_freq;
            h->M = m;
            // Перевстановлюємо частоти у 0, бо межі бінів повністю змінилися
            for (unsigned i = 0; i < m; i++) {
                h->frequency[i] = 0;
            }
            h->total = 0;
        }
    }
}

void addNumber(Histogram* h, double x, int clip_mode) {
    if (!h || h->M == 0) return;
    double w = (h->max_hist - h->min_hist) / h->M;
    if (x < h->min_hist) {
        if (clip_mode == 2) {
            h->frequency[0]++;
            h->total++;
        }
    } else if (x > h->max_hist) {
        if (clip_mode == 2) {
            h->frequency[h->M - 1]++;
            h->total++;
        }
    } else {
        unsigned i = (unsigned)((x - h->min_hist) / w);
        if (i >= h->M) i = h->M - 1;
        h->frequency[i]++;
        h->total++;
    }
}

void addBatch(Histogram* h, double* data, size_t dataSize, int clip_mode) {
    if (!h || !data) return;
    for (size_t i = 0; i < dataSize; i++) {
        addNumber(h, data[i], clip_mode);
    }
}

int addFromTextFile(Histogram* h, const char* filename, int clip_mode) {
    if (!h || !filename) return -1;
    FILE* f = fopen(filename, "r");
    if (!f) return -1;
    double val;
    int count = 0;
    while (fscanf(f, "%lf", &val) == 1) {
        addNumber(h, val, clip_mode);
        count++;
    }
    fclose(f);
    return count;
}

int addFromBinaryFile(Histogram* h, const char* filename, int clip_mode) {
    if (!h || !filename) return -1;
    FILE* f = fopen(filename, "rb");
    if (!f) return -1;
    double val;
    int count = 0;
    while (fread(&val, sizeof(double), 1, f) == 1) {
        addNumber(h, val, clip_mode);
        count++;
    }
    fclose(f);
    return count;
}

unsigned long num(const Histogram* h) {
    return h ? h->total : 0;
}

unsigned numHist(const Histogram* h, unsigned i) {
    if (h && i < h->M) {
        return h->frequency[i];
    }
    return 0;
}

double mean(const Histogram* h) {
    if (!h || h->total == 0 || h->M == 0) return 0.0;
    double w = (h->max_hist - h->min_hist) / h->M;
    double sum = 0.0;
    for (unsigned i = 0; i < h->M; i++) {
        double mid = h->min_hist + (i + 0.5) * w;
        sum += h->frequency[i] * mid;
    }
    return sum / (double)h->total;
}

double median(const Histogram* h) {
    if (!h || h->total == 0 || h->M == 0) return 0.0;
    double w = (h->max_hist - h->min_hist) / h->M;
    double target = (double)h->total / 2.0;
    double cum = 0.0;
    unsigned k = 0;
    for (k = 0; k < h->M; k++) {
        cum += h->frequency[k];
        if (cum >= target) {
            break;
        }
    }
    if (k == h->M) k = h->M - 1;
    double prev_cum = 0.0;
    for (unsigned j = 0; j < k; j++) {
        prev_cum += h->frequency[j];
    }
    double left_k = h->min_hist + k * w;
    if (h->frequency[k] > 0) {
        return left_k + w * ((target - prev_cum) / (double)h->frequency[k]);
    } else {
        return left_k + 0.5 * w;
    }
}

double dev(const Histogram* h) {
    if (!h || h->total == 0 || h->M == 0) return 0.0;
    double m = mean(h);
    double w = (h->max_hist - h->min_hist) / h->M;
    double sum = 0.0;
    for (unsigned i = 0; i < h->M; i++) {
        double mid = h->min_hist + (i + 0.5) * w;
        sum += h->frequency[i] * fabs(mid - m);
    }
    return sum / (double)h->total;
}

double variance(const Histogram* h) {
    if (!h || h->total == 0 || h->M == 0) return 0.0;
    double m = mean(h);
    double w = (h->max_hist - h->min_hist) / h->M;
    double sum = 0.0;
    for (unsigned i = 0; i < h->M; i++) {
        double mid = h->min_hist + (i + 0.5) * w;
        double diff = mid - m;
        sum += h->frequency[i] * diff * diff;
    }
    return sum / (double)h->total;
}

double skewness(const Histogram* h) {
    if (!h || h->total == 0 || h->M == 0) return 0.0;
    double m = mean(h);
    double var = variance(h);
    if (var < 1e-9) return 0.0;
    double sd = sqrt(var);
    double w = (h->max_hist - h->min_hist) / h->M;
    double sum = 0.0;
    for (unsigned i = 0; i < h->M; i++) {
        double mid = h->min_hist + (i + 0.5) * w;
        double diff = mid - m;
        sum += h->frequency[i] * diff * diff * diff;
    }
    double m3 = sum / (double)h->total;
    return m3 / (sd * sd * sd);
}

double kurtosis(const Histogram* h) {
    if (!h || h->total == 0 || h->M == 0) return 0.0;
    double m = mean(h);
    double var = variance(h);
    if (var < 1e-9) return 0.0;
    double w = (h->max_hist - h->min_hist) / h->M;
    double sum = 0.0;
    for (unsigned i = 0; i < h->M; i++) {
        double mid = h->min_hist + (i + 0.5) * w;
        double diff = mid - m;
        sum += h->frequency[i] * diff * diff * diff * diff;
    }
    double m4 = sum / (double)h->total;
    return m4 / (var * var) - 3.0;
}

double entropy(const Histogram* h) {
    if (!h || h->total == 0 || h->M == 0) return 0.0;
    double ent = 0.0;
    double tot = (double)h->total;
    for (unsigned i = 0; i < h->M; i++) {
        if (h->frequency[i] > 0) {
            double p = h->frequency[i] / tot;
            ent -= p * log(p);
        }
    }
    return ent;
}

int testNormal(const Histogram* h) {
    if (!h || h->total == 0 || h->M < 4) return 0;
    double mu = mean(h);
    double var = variance(h);
    if (var < 1e-9) return 0;
    double sd = sqrt(var);
    double w = (h->max_hist - h->min_hist) / h->M;
    double chi2 = 0.0;
    for (unsigned i = 0; i < h->M; i++) {
        double x_i = h->min_hist + i * w;
        double x_ip1 = h->min_hist + (i + 1) * w;
        double p;
        if (i == 0) {
            p = phi_std((x_ip1 - mu) / sd);
        } else if (i == h->M - 1) {
            p = 1.0 - phi_std((x_i - mu) / sd);
        } else {
            p = phi_std((x_ip1 - mu) / sd) - phi_std((x_i - mu) / sd);
        }
        if (p < 0.0) p = 0.0;
        double E = h->total * p;
        double O = h->frequency[i];
        if (E > 1e-9) {
            chi2 += (O - E) * (O - E) / E;
        }
    }
    double df = (double)h->M - 3.0;
    double p_value = gammq(df / 2.0, chi2 / 2.0);
    return (p_value >= 0.05) ? 1 : 0;
}

int testExponential(const Histogram* h) {
    if (!h || h->total == 0 || h->M < 3) return 0;
    double mu = mean(h);
    double offset_mean = mu - h->min_hist;
    if (offset_mean <= 1e-9) return 0;
    double lambda = 1.0 / offset_mean;
    double w = (h->max_hist - h->min_hist) / h->M;
    double chi2 = 0.0;
    for (unsigned i = 0; i < h->M; i++) {
        double x_i = h->min_hist + i * w;
        double x_ip1 = h->min_hist + (i + 1) * w;
        double p;
        if (i == h->M - 1) {
            p = exp(-lambda * (x_i - h->min_hist));
        } else {
            p = exp(-lambda * (x_i - h->min_hist)) - exp(-lambda * (x_ip1 - h->min_hist));
        }
        if (p < 0.0) p = 0.0;
        double E = h->total * p;
        double O = h->frequency[i];
        if (E > 1e-9) {
            chi2 += (O - E) * (O - E) / E;
        }
    }
    double df = (double)h->M - 2.0;
    double p_value = gammq(df / 2.0, chi2 / 2.0);
    return (p_value >= 0.05) ? 1 : 0;
}

int testUniform(const Histogram* h) {
    if (!h || h->total == 0 || h->M < 2) return 0;
    double p = 1.0 / h->M;
    double E = h->total * p;
    double chi2 = 0.0;
    for (unsigned i = 0; i < h->M; i++) {
        double O = (double)h->frequency[i];
        chi2 += (O - E) * (O - E) / E;
    }
    double df = (double)h->M - 1.0;
    double p_value = gammq(df / 2.0, chi2 / 2.0);
    return (p_value >= 0.05) ? 1 : 0;
}

int testBinomial(const Histogram* h, unsigned n_trials, double p) {
    if (!h || h->total == 0 || h->M < 2 || p < 0.0 || p > 1.0) return 0;
    double* probs = (double*)calloc(h->M, sizeof(double));
    if (!probs) return 0;
    
    double w = (h->max_hist - h->min_hist) / h->M;
    for (unsigned k = 0; k <= n_trials; k++) {
        double pk = binom_pmf(k, n_trials, p);
        unsigned bin_idx = 0;
        if ((double)k < h->min_hist) {
            bin_idx = 0;
        } else if ((double)k >= h->max_hist) {
            bin_idx = h->M - 1;
        } else {
            bin_idx = (unsigned)(((double)k - h->min_hist) / w);
            if (bin_idx >= h->M) bin_idx = h->M - 1;
        }
        probs[bin_idx] += pk;
    }
    
    double chi2 = 0.0;
    for (unsigned i = 0; i < h->M; i++) {
        double E = h->total * probs[i];
        double O = (double)h->frequency[i];
        if (E > 1e-9) {
            chi2 += (O - E) * (O - E) / E;
        }
    }
    free(probs);
    double df = (double)h->M - 1.0;
    double p_value = gammq(df / 2.0, chi2 / 2.0);
    return (p_value >= 0.05) ? 1 : 0;
}

int testPoisson(const Histogram* h, double lambda) {
    if (!h || h->total == 0 || h->M < 2 || lambda <= 0.0) return 0;
    double* probs = (double*)calloc(h->M, sizeof(double));
    if (!probs) return 0;
    
    double w = (h->max_hist - h->min_hist) / h->M;
    unsigned k = 0;
    while (1) {
        double pk = poisson_pmf(k, lambda);
        unsigned bin_idx = 0;
        if ((double)k < h->min_hist) {
            bin_idx = 0;
        } else if ((double)k >= h->max_hist) {
            bin_idx = h->M - 1;
        } else {
            bin_idx = (unsigned)(((double)k - h->min_hist) / w);
            if (bin_idx >= h->M) bin_idx = h->M - 1;
        }
        probs[bin_idx] += pk;
        
        if ((double)k > 3.0 * lambda && pk < 1e-10) {
            break;
        }
        k++;
        if (k > 100000) break;
    }
    
    double chi2 = 0.0;
    for (unsigned i = 0; i < h->M; i++) {
        double E = h->total * probs[i];
        double O = (double)h->frequency[i];
        if (E > 1e-9) {
            chi2 += (O - E) * (O - E) / E;
        }
    }
    free(probs);
    double df = (double)h->M - 1.0;
    double p_value = gammq(df / 2.0, chi2 / 2.0);
    return (p_value >= 0.05) ? 1 : 0;
}

void printHistogram(const Histogram* h) {
    if (!h || h->M == 0) {
        printf("Гістограма порожня або неініціалізована.\n");
        return;
    }
    printf("\n=== ГІСТОГРАМА ЧАСТОТ (всього чисел: %lu) ===\n", h->total);
    double w = (h->max_hist - h->min_hist) / h->M;
    unsigned max_freq = 0;
    for (unsigned i = 0; i < h->M; i++) {
        if (h->frequency[i] > max_freq) {
            max_freq = h->frequency[i];
        }
    }
    for (unsigned i = 0; i < h->M; i++) {
        double left = h->min_hist + i * w;
        double right = h->min_hist + (i + 1) * w;
        printf("[%6.2f, %6.2f) | ", left, right);
        
        unsigned bar_len = 0;
        if (max_freq > 0) {
            bar_len = (h->frequency[i] * 40) / max_freq;
        }
        for (unsigned j = 0; j < bar_len; j++) {
            putchar('#');
        }
        for (unsigned j = bar_len; j < 40; j++) {
            putchar(' ');
        }
        double pct = (h->total > 0) ? (100.0 * h->frequency[i] / h->total) : 0.0;
        printf(" | %-8u [%5.1f%%]\n", h->frequency[i], pct);
    }
    printf("=============================================\n\n");
}

int saveToTextFile(const Histogram* h, const char* filename) {
    if (!h || !filename) return 0;
    FILE* f = fopen(filename, "w");
    if (!f) return 0;
    fprintf(f, "%f %f %u %lu\n", h->min_hist, h->max_hist, h->M, h->total);
    for (unsigned i = 0; i < h->M; i++) {
        fprintf(f, "%u%c", h->frequency[i], (i == h->M - 1) ? '\n' : ' ');
    }
    fclose(f);
    return 1;
}

int loadFromTextFile(Histogram* h, const char* filename) {
    if (!h || !filename) return 0;
    FILE* f = fopen(filename, "r");
    if (!f) return 0;
    double min_h = 0, max_h = 0;
    unsigned M = 0;
    unsigned long tot = 0;
    if (fscanf(f, "%lf %lf %u %lu", &min_h, &max_h, &M, &tot) != 4) {
        fclose(f);
        return 0;
    }
    if (h->frequency) {
        Histogram_free(h);
    }
    if (Histogram_init(h, M, min_h, max_h) != 0) {
        fclose(f);
        return 0;
    }
    h->total = tot;
    for (unsigned i = 0; i < M; i++) {
        if (fscanf(f, "%u", &h->frequency[i]) != 1) {
            fclose(f);
            return 0;
        }
    }
    fclose(f);
    return 1;
}

int saveToBinaryFile(const Histogram* h, const char* filename) {
    if (!h || !filename) return 0;
    FILE* f = fopen(filename, "wb");
    if (!f) return 0;
    fwrite(&h->min_hist, sizeof(double), 1, f);
    fwrite(&h->max_hist, sizeof(double), 1, f);
    fwrite(&h->M, sizeof(unsigned), 1, f);
    fwrite(&h->total, sizeof(unsigned long), 1, f);
    fwrite(h->frequency, sizeof(unsigned), h->M, f);
    fclose(f);
    return 1;
}

int loadFromBinaryFile(Histogram* h, const char* filename) {
    if (!h || !filename) return 0;
    FILE* f = fopen(filename, "rb");
    if (!f) return 0;
    double min_h = 0, max_h = 0;
    unsigned M = 0;
    unsigned long tot = 0;
    if (fread(&min_h, sizeof(double), 1, f) != 1 ||
        fread(&max_h, sizeof(double), 1, f) != 1 ||
        fread(&M, sizeof(unsigned), 1, f) != 1 ||
        fread(&tot, sizeof(unsigned long), 1, f) != 1) {
        fclose(f);
        return 0;
    }
    if (h->frequency) {
        Histogram_free(h);
    }
    if (Histogram_init(h, M, min_h, max_h) != 0) {
        fclose(f);
        return 0;
    }
    h->total = tot;
    if (fread(h->frequency, sizeof(unsigned), M, f) != M) {
        fclose(f);
        return 0;
    }
    fclose(f);
    return 1;
}

int inputFromString(Histogram* h, const char* str) {
    if (!h || !str) return 0;
    const char* ptr = str;
    char* endptr;
    int total_chars = 0;
    while (*ptr != '\0') {
        while (*ptr && (isspace((unsigned char)*ptr) || *ptr == ',' || *ptr == ';')) {
            ptr++;
        }
        if (*ptr == '\0') break;
        double val = strtod(ptr, &endptr);
        if (endptr == ptr) {
            break;
        }
        addNumber(h, val, 1);
        total_chars = (int)(endptr - str);
        ptr = endptr;
    }
    return total_chars;
}
