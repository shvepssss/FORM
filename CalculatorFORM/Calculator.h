#pragma once
#include <exception>
#include <cstdlib>
#include <cstring>
#include <iostream>

using namespace std;

class Calculator
{
public:
	~Calculator();
	// расчет значения выражения 
	double calc(const char* str) throw(runtime_error);

private:
	// получить строку без пробелов
	void get_str_ws(const char* str, char* str_ws);
	// проверка ошибок в выражении
	bool has_errors(const char* str);
	// проверка наличия . в числе
	int check_point(const char* str, int size);
	// преобразование массива символов в число
	double str_to_double(const char* str, int size);
	// считываем числа из выражения
	size_t read_digits(const char* str);
	// получаем максимальный приоритет действий
	size_t get_max_priority(size_t size);
	// получаем максимальное количество подряд идущих степеней в строке, из тех, что еще не считаны
	size_t get_max_pow(const char* str, size_t shift);
	// считаем действия из выражения
	size_t read_operators(const char* str);
	// ищем индекс максимального приоритета действия
	size_t get_index_max_priority(size_t size);
	// производим рассчёт
	double get_answer(size_t &size_digits, size_t &size_operators);
	// изменяем массивы приоритетов, операторов и чисел
	void memory_reallocation(size_t size, size_t index, double new_val);
	// очищаем массивы
	void clear_data();

	// числа
	double *digits;
	// операторы (действия в выражении)
	char *operators;
	// приоритеты операторов
	int *priorities;
};

