/*
 * File: Histogram.cpp
 * Task: Histogram — структура для статистичної обробки даних (завдання 13)
 * Group: К-24
 * Author: Курлов Микита
 * Date: 2026-05-19
 * Description: Реалізація методів C++ класу Histogram.
 *              Використовує анонімний простір імен для допоміжних математичних
 *              функцій та регулярної неповної гамма-функції.
 */

#include "Histogram.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <cctype>

/* ========================================================================= *
 *                       ВНУТРІШНІ ДОПОМІЖНІ ФУНКЦІЇ                         *
 * ========================================================================= */

namespace {

/**
 * @brief Ланцош наближення для ln(Г(x)).
 */
double log_gamma(double xx) {
    double x = xx - 1.0;
    double tmp = x + 5.5;
    tmp -= (x + 0.5) * std::log(tmp);
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
    return -tmp + std::log(2.5066282746310005 * ser);
}

/**
 * @brief Обчислює неповну гамма-функцію через ряд P(a, x).
 */
double gser(double a, double x) {
    if (x <= 0.0) return 0.0;
    double sum = 1.0 / a;
    double del = sum;
    double ap = a;
    for (int n = 1; n <= 100; n++) {
        ap += 1.0;
        del *= x / ap;
        sum += del;
        if (std::fabs(del) < std::fabs(sum) * 3e-7) break;
    }
    return sum * std::exp(-x + a * std::log(x) - log_gamma(a));
}

/**
 * @brief Обчислює неповну гамма-функцію через ланцюговий дріб Q(a, x).
 */
double gcf(double a, double x) {
    double b = x + 1.0 - a;
    double c = 1.0 / 1e-30;
    double d = 1.0 / b;
    double h = d;
    for (int i = 1; i <= 100; i++) {
        double an = -i * (i - a);
        b += 2.0;
        d = an * d + b;
        if (std::fabs(d) < 1e-30) d = 1e-30;
        c = b + an / c;
        if (std::fabs(c) < 1e-30) c = 1e-30;
        d = 1.0 / d;
        double del = d * c;
        h *= del;
        if (std::fabs(del - 1.0) < 3e-7) break;
    }
    return h * std::exp(-x + a * std::log(x) - log_gamma(a));
}

/**
 * @brief Регулярна неповна гамма-функція Q(a, x) = 1 - P(a, x).
 */
double gammq(double a, double x) {
    if (x < 0.0 || a <= 0.0) return 0.0;
    if (x < a + 1.0) {
        return 1.0 - gser(a, x);
    } else {
        return gcf(a, x);
    }
}

/**
 * @brief Стандартний нормальний інтеграл ймовірностей Ф(z).
 */
double phi_std(double z) {
    return 0.5 * (1.0 + std::erf(z / std::sqrt(2.0)));
}

/**
 * @brief Біноміальний PMF P(X=k).
 */
double binom_pmf(unsigned k, unsigned n, double p) {
    if (k > n || p < 0.0 || p > 1.0) return 0.0;
    if (p == 0.0) return (k == 0) ? 1.0 : 0.0;
    if (p == 1.0) return (k == n) ? 1.0 : 0.0;
    double log_comb = log_gamma(n + 1.0) - log_gamma(k + 1.0) - log_gamma(n - k + 1.0);
    double log_prob = log_comb + k * std::log(p) + (n - k) * std::log(1.0 - p);
    return std::exp(log_prob);
}

/**
 * @brief PMF розподілу Пуассона P(X=k).
 */
double poisson_pmf(unsigned k, double lambda) {
    if (lambda <= 0.0) return 0.0;
    double log_prob = (double)k * std::log(lambda) - lambda - log_gamma((double)k + 1.0);
    return std::exp(log_prob);
}

} // namespace

/* ========================================================================= *
 *                        МЕТОДИ КЛАСУ HISTOGRAM                             *
 * ========================================================================= */

namespace cpp {

Histogram::Histogram(unsigned M, double min, double max)
    : min_hist(min), max_hist(max), M(M), total(0) {
    if (M == 0) this->M = 10;
    if (min_hist >= max_hist) {
        min_hist = 0.0;
        max_hist = 100.0;
    }
    frequency = new unsigned[this->M]();
}

Histogram::~Histogram() {
    delete[] frequency;
}

void Histogram::setMax(double m) {
    if (m > min_hist) {
        max_hist = m;
    }
}

void Histogram::setMin(double m) {
    if (m < max_hist) {
        min_hist = m;
    }
}

void Histogram::setM(unsigned m) {
    if (m == 0) return;
    if (M != m) {
        delete[] frequency;
        M = m;
        frequency = new unsigned[M]();
        total = 0;
    }
}

void Histogram::addNumber(double x, int clip_mode) {
    if (M == 0) return;
    double w = (max_hist - min_hist) / M;
    if (x < min_hist) {
        if (clip_mode == 2) {
            frequency[0]++;
            total++;
        }
    } else if (x > max_hist) {
        if (clip_mode == 2) {
            frequency[M - 1]++;
            total++;
        }
    } else {
        unsigned i = (unsigned)((x - min_hist) / w);
        if (i >= M) i = M - 1;
        frequency[i]++;
        total++;
    }
}

void Histogram::addBatch(double* data, std::size_t dataSize, int clip_mode) {
    if (!data) return;
    for (std::size_t i = 0; i < dataSize; i++) {
        addNumber(data[i], clip_mode);
    }
}

int Histogram::addFromTextFile(const char* filename, int clip_mode) {
    if (!filename) return -1;
    std::ifstream f(filename);
    if (!f.is_open()) return -1;
    double val;
    int count = 0;
    while (f >> val) {
        addNumber(val, clip_mode);
        count++;
    }
    return count;
}

int Histogram::addFromBinaryFile(const char* filename, int clip_mode) {
    if (!filename) return -1;
    std::ifstream f(filename, std::ios::binary);
    if (!f.is_open()) return -1;
    double val;
    int count = 0;
    while (f.read(reinterpret_cast<char*>(&val), sizeof(double))) {
        addNumber(val, clip_mode);
        count++;
    }
    return count;
}

unsigned long Histogram::num() const {
    return total;
}

unsigned Histogram::numHist(unsigned i) const {
    if (i < M) {
        return frequency[i];
    }
    return 0;
}

double Histogram::mean() const {
    if (total == 0 || M == 0) return 0.0;
    double w = (max_hist - min_hist) / M;
    double sum = 0.0;
    for (unsigned i = 0; i < M; i++) {
        double mid = min_hist + (i + 0.5) * w;
        sum += frequency[i] * mid;
    }
    return sum / (double)total;
}

double Histogram::median() const {
    if (total == 0 || M == 0) return 0.0;
    double w = (max_hist - min_hist) / M;
    double target = (double)total / 2.0;
    double cum = 0.0;
    unsigned k = 0;
    for (k = 0; k < M; k++) {
        cum += frequency[k];
        if (cum >= target) {
            break;
        }
    }
    if (k == M) k = M - 1;
    double prev_cum = 0.0;
    for (unsigned j = 0; j < k; j++) {
        prev_cum += frequency[j];
    }
    double left_k = min_hist + k * w;
    if (frequency[k] > 0) {
        return left_k + w * ((target - prev_cum) / (double)frequency[k]);
    } else {
        return left_k + 0.5 * w;
    }
}

double Histogram::dev() const {
    if (total == 0 || M == 0) return 0.0;
    double m = mean();
    double w = (max_hist - min_hist) / M;
    double sum = 0.0;
    for (unsigned i = 0; i < M; i++) {
        double mid = min_hist + (i + 0.5) * w;
        sum += frequency[i] * std::fabs(mid - m);
    }
    return sum / (double)total;
}

double Histogram::variance() const {
    if (total == 0 || M == 0) return 0.0;
    double m = mean();
    double w = (max_hist - min_hist) / M;
    double sum = 0.0;
    for (unsigned i = 0; i < M; i++) {
        double mid = min_hist + (i + 0.5) * w;
        double diff = mid - m;
        sum += frequency[i] * diff * diff;
    }
    return sum / (double)total;
}

double Histogram::skewness() const {
    if (total == 0 || M == 0) return 0.0;
    double m = mean();
    double var = variance();
    if (var < 1e-9) return 0.0;
    double sd = std::sqrt(var);
    double w = (max_hist - min_hist) / M;
    double sum = 0.0;
    for (unsigned i = 0; i < M; i++) {
        double mid = min_hist + (i + 0.5) * w;
        double diff = mid - m;
        sum += frequency[i] * diff * diff * diff;
    }
    double m3 = sum / (double)total;
    return m3 / (sd * sd * sd);
}

double Histogram::kurtosis() const {
    if (total == 0 || M == 0) return 0.0;
    double m = mean();
    double var = variance();
    if (var < 1e-9) return 0.0;
    double w = (max_hist - min_hist) / M;
    double sum = 0.0;
    for (unsigned i = 0; i < M; i++) {
        double mid = min_hist + (i + 0.5) * w;
        double diff = mid - m;
        sum += frequency[i] * diff * diff * diff * diff;
    }
    double m4 = sum / (double)total;
    return m4 / (var * var) - 3.0;
}

double Histogram::entropy() const {
    if (total == 0 || M == 0) return 0.0;
    double ent = 0.0;
    double tot = (double)total;
    for (unsigned i = 0; i < M; i++) {
        if (frequency[i] > 0) {
            double p = frequency[i] / tot;
            ent -= p * std::log(p);
        }
    }
    return ent;
}

int Histogram::testNormal() const {
    if (total == 0 || M < 4) return 0;
    double mu = mean();
    double var = variance();
    if (var < 1e-9) return 0;
    double sd = std::sqrt(var);
    double w = (max_hist - min_hist) / M;
    double chi2 = 0.0;
    for (unsigned i = 0; i < M; i++) {
        double x_i = min_hist + i * w;
        double x_ip1 = min_hist + (i + 1) * w;
        double p;
        if (i == 0) {
            p = phi_std((x_ip1 - mu) / sd);
        } else if (i == M - 1) {
            p = 1.0 - phi_std((x_i - mu) / sd);
        } else {
            p = phi_std((x_ip1 - mu) / sd) - phi_std((x_i - mu) / sd);
        }
        if (p < 0.0) p = 0.0;
        double E = total * p;
        double O = frequency[i];
        if (E > 1e-9) {
            chi2 += (O - E) * (O - E) / E;
        }
    }
    double df = (double)M - 3.0;
    double p_value = gammq(df / 2.0, chi2 / 2.0);
    return (p_value >= 0.05) ? 1 : 0;
}

int Histogram::testExponential() const {
    if (total == 0 || M < 3) return 0;
    double mu = mean();
    double offset_mean = mu - min_hist;
    if (offset_mean <= 1e-9) return 0;
    double lambda = 1.0 / offset_mean;
    double w = (max_hist - min_hist) / M;
    double chi2 = 0.0;
    for (unsigned i = 0; i < M; i++) {
        double x_i = min_hist + i * w;
        double x_ip1 = min_hist + (i + 1) * w;
        double p;
        if (i == M - 1) {
            p = std::exp(-lambda * (x_i - min_hist));
        } else {
            p = std::exp(-lambda * (x_i - min_hist)) - std::exp(-lambda * (x_ip1 - min_hist));
        }
        if (p < 0.0) p = 0.0;
        double E = total * p;
        double O = frequency[i];
        if (E > 1e-9) {
            chi2 += (O - E) * (O - E) / E;
        }
    }
    double df = (double)M - 2.0;
    double p_value = gammq(df / 2.0, chi2 / 2.0);
    return (p_value >= 0.05) ? 1 : 0;
}

int Histogram::testUniform() const {
    if (total == 0 || M < 2) return 0;
    double p = 1.0 / M;
    double E = total * p;
    double chi2 = 0.0;
    for (unsigned i = 0; i < M; i++) {
        double O = (double)frequency[i];
        chi2 += (O - E) * (O - E) / E;
    }
    double df = (double)M - 1.0;
    double p_value = gammq(df / 2.0, chi2 / 2.0);
    return (p_value >= 0.05) ? 1 : 0;
}

int Histogram::testBinomial(unsigned n_trials, double p) const {
    if (total == 0 || M < 2 || p < 0.0 || p > 1.0) return 0;
    double* probs = new double[M]();
    
    double w = (max_hist - min_hist) / M;
    for (unsigned k = 0; k <= n_trials; k++) {
        double pk = binom_pmf(k, n_trials, p);
        unsigned bin_idx = 0;
        if ((double)k < min_hist) {
            bin_idx = 0;
        } else if ((double)k >= max_hist) {
            bin_idx = M - 1;
        } else {
            bin_idx = (unsigned)(((double)k - min_hist) / w);
            if (bin_idx >= M) bin_idx = M - 1;
        }
        probs[bin_idx] += pk;
    }
    
    double chi2 = 0.0;
    for (unsigned i = 0; i < M; i++) {
        double E = total * probs[i];
        double O = (double)frequency[i];
        if (E > 1e-9) {
            chi2 += (O - E) * (O - E) / E;
        }
    }
    delete[] probs;
    double df = (double)M - 1.0;
    double p_value = gammq(df / 2.0, chi2 / 2.0);
    return (p_value >= 0.05) ? 1 : 0;
}

int Histogram::testPoisson(double lambda) const {
    if (total == 0 || M < 2 || lambda <= 0.0) return 0;
    double* probs = new double[M]();
    
    double w = (max_hist - min_hist) / M;
    unsigned k = 0;
    while (true) {
        double pk = poisson_pmf(k, lambda);
        unsigned bin_idx = 0;
        if ((double)k < min_hist) {
            bin_idx = 0;
        } else if ((double)k >= max_hist) {
            bin_idx = M - 1;
        } else {
            bin_idx = (unsigned)(((double)k - min_hist) / w);
            if (bin_idx >= M) bin_idx = M - 1;
        }
        probs[bin_idx] += pk;
        
        if ((double)k > 3.0 * lambda && pk < 1e-10) {
            break;
        }
        k++;
        if (k > 100000) break;
    }
    
    double chi2 = 0.0;
    for (unsigned i = 0; i < M; i++) {
        double E = total * probs[i];
        double O = (double)frequency[i];
        if (E > 1e-9) {
            chi2 += (O - E) * (O - E) / E;
        }
    }
    delete[] probs;
    double df = (double)M - 1.0;
    double p_value = gammq(df / 2.0, chi2 / 2.0);
    return (p_value >= 0.05) ? 1 : 0;
}

void Histogram::printHistogram() const {
    if (M == 0) {
        std::cout << "Гістограма порожня або неініціалізована.\n";
        return;
    }
    std::cout << "\n=== ГІСТОГРАМА ЧАСТОТ C++ (всього чисел: " << total << ") ===\n";
    double w = (max_hist - min_hist) / M;
    unsigned max_freq = 0;
    for (unsigned i = 0; i < M; i++) {
        if (frequency[i] > max_freq) {
            max_freq = frequency[i];
        }
    }
    for (unsigned i = 0; i < M; i++) {
        double left = min_hist + i * w;
        double right = min_hist + (i + 1) * w;
        std::cout << "[" << std::setw(6) << std::fixed << std::setprecision(2) << left << ", "
                  << std::setw(6) << right << ") | ";
        
        unsigned bar_len = 0;
        if (max_freq > 0) {
            bar_len = (frequency[i] * 40) / max_freq;
        }
        for (unsigned j = 0; j < bar_len; j++) {
            std::cout << '#';
        }
        for (unsigned j = bar_len; j < 40; j++) {
            std::cout << ' ';
        }
        double pct = (total > 0) ? (100.0 * frequency[i] / total) : 0.0;
        std::cout << " | " << std::setw(8) << std::left << frequency[i]
                  << " [" << std::setw(5) << std::right << std::setprecision(1) << pct << "%]\n";
    }
    std::cout << "=================================================\n\n";
}

int Histogram::saveToTextFile(const char* filename) const {
    if (!filename) return 0;
    std::ofstream f(filename);
    if (!f.is_open()) return 0;
    f << std::setprecision(10)
      << min_hist << " " << max_hist << " " << M << " " << total << "\n";
    for (unsigned i = 0; i < M; i++) {
        f << frequency[i] << (i == M - 1 ? "" : " ");
    }
    f << "\n";
    return 1;
}

int Histogram::loadFromTextFile(const char* filename) {
    if (!filename) return 0;
    std::ifstream f(filename);
    if (!f.is_open()) return 0;
    double min_h = 0, max_h = 0;
    unsigned temp_M = 0;
    unsigned long tot = 0;
    if (!(f >> min_h >> max_h >> temp_M >> tot)) return 0;
    if (temp_M == 0 || min_h >= max_h) return 0;
    
    delete[] frequency;
    M = temp_M;
    min_hist = min_h;
    max_hist = max_h;
    total = tot;
    frequency = new unsigned[M]();
    
    for (unsigned i = 0; i < M; i++) {
        if (!(f >> frequency[i])) return 0;
    }
    return 1;
}

int Histogram::saveToBinaryFile(const char* filename) const {
    if (!filename) return 0;
    std::ofstream f(filename, std::ios::binary);
    if (!f.is_open()) return 0;
    f.write(reinterpret_cast<const char*>(&min_hist), sizeof(double));
    f.write(reinterpret_cast<const char*>(&max_hist), sizeof(double));
    f.write(reinterpret_cast<const char*>(&M), sizeof(unsigned));
    f.write(reinterpret_cast<const char*>(&total), sizeof(unsigned long));
    f.write(reinterpret_cast<const char*>(frequency), M * sizeof(unsigned));
    return 1;
}

int Histogram::loadFromBinaryFile(const char* filename) {
    if (!filename) return 0;
    std::ifstream f(filename, std::ios::binary);
    if (!f.is_open()) return 0;
    double min_h = 0, max_h = 0;
    unsigned temp_M = 0;
    unsigned long tot = 0;
    
    if (!f.read(reinterpret_cast<char*>(&min_h), sizeof(double)) ||
        !f.read(reinterpret_cast<char*>(&max_h), sizeof(double)) ||
        !f.read(reinterpret_cast<char*>(&temp_M), sizeof(unsigned)) ||
        !f.read(reinterpret_cast<char*>(&tot), sizeof(unsigned long))) {
        return 0;
    }
    if (temp_M == 0 || min_h >= max_h) return 0;
    
    delete[] frequency;
    M = temp_M;
    min_hist = min_h;
    max_hist = max_h;
    total = tot;
    frequency = new unsigned[M]();
    
    if (!f.read(reinterpret_cast<char*>(frequency), M * sizeof(unsigned))) {
        return 0;
    }
    return 1;
}

int Histogram::inputFromString(const char* str) {
    if (!str) return 0;
    const char* ptr = str;
    char* endptr;
    int total_chars = 0;
    while (*ptr != '\0') {
        while (*ptr && (std::isspace((unsigned char)*ptr) || *ptr == ',' || *ptr == ';')) {
            ptr++;
        }
        if (*ptr == '\0') break;
        double val = std::strtod(ptr, &endptr);
        if (endptr == ptr) {
            break;
        }
        addNumber(val, 1);
        total_chars = (int)(endptr - str);
        ptr = endptr;
    }
    return total_chars;
}

} // namespace cpp
