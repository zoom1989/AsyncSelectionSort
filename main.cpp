#include <iostream>
#include <vector>
#include <future>
#include <thread>
#include <chrono>
#include <algorithm>
#include <random>

// ---------------------------------------------------------
// Функция для поиска индекса минимального элемента в диапазоне [start, end)
// Возвращает пару: индекс минимального элемента и его значение
// ---------------------------------------------------------
std::pair<size_t, int> find_min_async(const std::vector<int>& arr, size_t start, size_t end) {
    if (start >= end) {
        return { start, INT_MAX };
    }

    size_t min_idx = start;
    int min_val = arr[start];

    for (size_t i = start + 1; i < end; ++i) {
        if (arr[i] < min_val) {
            min_val = arr[i];
            min_idx = i;
        }
    }
    return { min_idx, min_val };
}

// ---------------------------------------------------------
// Асинхронная функция поиска минимума с использованием promise/future
// (в учебных целях, чтобы показать передачу результата)
// ---------------------------------------------------------
std::future<std::pair<size_t, int>> async_find_min(const std::vector<int>& arr, size_t start, size_t end) {
    // Используем std::async с политикой запуска в отдельном потоке
    return std::async(std::launch::async, [&arr, start, end]() {
        return find_min_async(arr, start, end);
        });
}

// ---------------------------------------------------------
// Рекурсивная функция для асинхронной сортировки выбором
// ---------------------------------------------------------
void async_selection_sort(std::vector<int>& arr, size_t start) {
    if (start >= arr.size()) return;

    // Запускаем асинхронный поиск минимума для оставшейся части массива
    auto future_min = async_find_min(arr, start, arr.size());

    // Ожидаем результат из потока
    auto [min_idx, min_val] = future_min.get();

    // Меняем местами текущий элемент и найденный минимум
    if (min_idx != start) {
        std::swap(arr[start], arr[min_idx]);
    }

    // Рекурсивный запуск следующего шага (асинхронная задача порождает новую)
    // Для демонстрации "рекурсивного запуска асинхронных задач".
    // В реальной жизни мы бы просто вызвали рекурсивно, но здесь используем async
    // в учебных целях.
    if (start + 1 < arr.size()) {
        auto next_future = std::async(std::launch::async, [&arr, start]() {
            async_selection_sort(arr, start + 1);
            });
        next_future.get(); // Дожидаемся завершения следующего шага (это делает сортировку последовательной, но демонстрирует рекурсию)
    }
}

int main() {
    // 1. Генерируем случайный массив
    std::vector<int> arr = { 5, 2, 8, 1, 9, 3, 7, 4, 6 };
    std::cout << "Исходный массив: ";
    for (int x : arr) std::cout << x << " ";
    std::cout << std::endl;

    // 2. Запускаем сортировку (первый шаг в отдельном потоке)
    auto start_time = std::chrono::high_resolution_clock::now();

    // Запускаем весь процесс через асинхронную задачу
    auto sort_future = std::async(std::launch::async, [&arr]() {
        async_selection_sort(arr, 0);
        });

    // Ожидаем завершения сортировки
    sort_future.get();

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end_time - start_time;

    // 3. Выводим результат
    std::cout << "Отсортированный массив: ";
    for (int x : arr) std::cout << x << " ";
    std::cout << std::endl;

    std::cout << "Vremya sortirovki: " << diff.count() << " секунд" << std::endl;

    return 0;
}