#include "Calculator.h"

const int len = 500;

const char* error_content = "Выражение содержит ошибку, калькулятор не может его прочитать.";
const char* error_div_zero = "Выражение содержит деление числа на 0, рассчёт невозможен.";
const char* error_memory = "Программа не может выделить память объекту.";


/* Деструктор */
Calculator::~Calculator()
{
	// освобождаем память
	clear_data();
}

/* Расчет значения выражения */
double Calculator::calc(const char* str) throw(runtime_error)
{
	char* str_ws = (char*)malloc(len);
	operators = (char*)malloc(len);
	priorities = (int*)malloc(len * sizeof(int));
	digits = (double*)malloc(len * sizeof(double));

	if (operators == NULL || priorities == NULL || digits == NULL || str_ws == NULL)
	{
		throw runtime_error(error_memory);
	}

	// записываем строку без пробелов
    get_str_ws(str, str_ws);

	// проверяем выражение на ошибки
	if (has_errors(str_ws))
	{
		throw runtime_error(error_content);
	}

	// считываем числа
	size_t size_digits = read_digits(str_ws);
	// считываем знаки действий
	size_t size_operators = read_operators(str_ws);

	// проверка, что операторов действий меньше на 1, чем цифр, иначе в записи выражения ошибка
	if (size_operators != size_digits - 1)
	{
		throw runtime_error(error_content);
	}

	// производим рассчёт
	double answer = get_answer(size_digits, size_operators);
	free(str_ws);

	return answer;
}

/* Получить строку без пробелов */
void Calculator::get_str_ws(const char* str, char* str_ws)
{
	size_t i = 0;
	// последний символ -не пробел
	char last_val=' ';
	bool space = false;

	while (*str)
	{
		if (*str != ' ')
		{
			*str_ws = *str;
			++str_ws;

			// обрабатываем "8 8"
			if ((int)last_val >= 48 && (int)last_val <= 57 && (int)*str >= 48 && (int)*str <= 57 && space)
			{
				throw runtime_error(error_content);
			}
			last_val = *str;
			space = false;
		}
		else
		{
			space = true;
		}
		++str;
		++i;
	}
	*str_ws = 0;

	// возвращаемся к первому символу
	while (i > 0)
	{
		--str_ws;
		--i;
	}
}

/* Проверка ошибок в выражении. true - ошибки есть, false - ошибок нет */
bool Calculator::has_errors(const char* str)
{
	bool answer = false;
	size_t i=0, count_lbrackets = 0, count_rbrackets = 0, count_points = 0;

	// --5
	if (str[0] == '-' && str[1] == '-') 
		answer = true;

	while (*str)
	{
		// неравное кол-во ( и )
		if (*str == '(')
		{
			// обрабатываем пример 8(
			if (i > 0)
			{
				if (((int)*(str - 1) >= 48 && (int)*(str - 1) <= 57) || *(str - 1) == '.')
				{
					answer = true;
					break;
				}
			}
			count_lbrackets++;
		}
		if (*str == ')')
		{
			// обрабатываем пример )8
			if (((int)*(str + 1) >= 48 && (int)*(str + 1) <= 57) || *(str + 1) == '.')
			{
				answer = true;
				break;
			}
			count_rbrackets++;
		}
		// проверка на )(
		if (count_rbrackets > count_lbrackets)
		{
			answer = true;
			break;
		}
		
		// символы, кроме цифр и знаков
		if (!(((int)*str >= 48 && (int)*str <= 57) || *str == '.' || *str == '-'
			|| *str == '+' || *str == '*' || *str == '/' || *str == '^' 
			|| *str == '(' || *str == ')'))
		{
			answer = true;
			break;
		}

		// знаки действий подряд кроме *какой-то знак*-
		if (i > 0)
		{
			if (( *str == '+' || *str == '*' || *str == '/' || *str == '^')
				&& (*(str-1) == '-' || *(str - 1) == '+' || *(str - 1) == '*' || *(str - 1) == '/' || *(str - 1) == '^'))
			{
				answer = true;
				break;
			}
		}

		// проверка на 5.5.5 
		if (*str == '.')
		{
			count_points++;
		}
		if (( * str == '-' || *str == '+' || *str == '*' || *str == '/' || *str == '^' || *str == ')') && count_points>0)
		{
			count_points--;
		}
		// если кол-во подряд идущих точек между цифрами больше одной
		if (count_points>1)
		{
			answer = true;
			break;
		}

		++i;
		++str;
	}

	if(count_rbrackets != count_lbrackets)
		answer = true;

	// 5--
	if (*(str-1) == '-' && *(str - 2) == '-')
		answer = true;
	
	
	return answer;
}

/* Проверка наличия . в числе */
int Calculator::check_point(const char* str, int size)
{
	for (int i = 0; i < size; ++i)
	{
		if (str[i] == '.')
		{
			return i;
		}
	}
	
	return -1;
}

/* Преобразование массива символов в число */
double Calculator::str_to_double(const char* str, int size)
{
	// определяем, есть ли точка в записи числа
	int pozition_point = check_point(str, size);
	double digit= 0; // получаемое число
	double coef = 1; // коэффициент для расчета 

	// высчитываем коеффициент для корректного расчета части числа справа от точки
	if (pozition_point != -1)
	{
		for (size_t j = 0; j < size- pozition_point - 1; j++)
		{
			coef *= 0.1;
		}
	}

	// высчитываем число
	for (int i = size-1; i>=0;i--)
	{
		if ((int)str[i] >= 48 && (int)str[i] <= 57) // если символ это число
		{
			digit += (str[i]-48) * coef;
			coef *= 10;
		}
		else if (str[i] == '-' && i==0) // если символ это знак отрицания
		{
			digit *= -1;
		}
	}

	return digit;
}

/* Считываем числа из выражения */
size_t Calculator::read_digits(const char* str)
{
	char* digit_str; // считываемое число
	digit_str = (char*)malloc(len);

	if (digit_str == NULL)
	{
		throw runtime_error(error_memory);
	}

	size_t i = 0; // счетчик для массива чисел
	size_t j = 0; // счетчик для массива-будущего числа

	while(*str)
	{
		// числа . -
		if (((int)*str >= 48 && (int)*str <= 57) || *str == '.' || *str == '-')
		{
			// проводим обработку для двоичного значения
			if (*str == '.')
			{
				// проверяем на недостаток цифр слева от .
				if (*(str-1) == NULL)
				{
					digit_str[j++] = '0';
					digit_str[j++] = '.';
				}
				else
				{
					if ((int)*(str - 1) >= 48 && (int)*(str - 1) <= 57)
					{
						digit_str[j++] = *str;
					}
					else
					{
						digit_str[j++] = '0';
						digit_str[j++] = '.';
					}
				}
				// проверяем на недостаток цифр справа от .
				if (*(str + 1) == NULL || !((int)*(str + 1) >= 48 && (int)*(str + 1) <= 57))
				{
					digit_str[j++] = '0';
				}
			}
			else if(*str == '-')
			{
				// если слева пусто, или не цифра(и не точка!) - то это отрицательное число
				if (*(str - 1) == NULL || (!((int)*(str - 1) >= 48 && (int)*(str - 1) <= 57) && *(str - 1)!='.' && *(str - 1) != ')'))
				{
					digit_str[j++] = *str;
				}
				else
				{
					digits[i] = str_to_double(digit_str, j);
					i++;
					free(digit_str);
					j = 0;
					digit_str = (char*)malloc(len);
					if (digit_str == NULL)
					{
						throw runtime_error(error_memory);
					}
				}
			}
			else
			{
				digit_str[j++] = *str;
			}
		}
		// иначе это знак действия, если (j==0 && i==0 - и это знак, то это скорее всего знак в начале выражения, что является ошибкой
		else if (*str != '(' && *str != ')' && !(j==0 && i==0))
		{
			digits[i] = str_to_double(digit_str, j);
			i++;
			free(digit_str);
			j = 0;
			digit_str = (char*)malloc(len);
			if (digit_str == NULL)
			{
				throw runtime_error(error_memory);
			}
		}
		++str;
	}

	// добавляем последнее число, если оно есть
	if (j != 0)
	{
		digits[i] = str_to_double(digit_str, j);
		i++;
	}
	free(digit_str);
	j = 0;

	// возвращаем размер массива
	return i;
}

/* Получаем максимальный приоритет действий */
size_t Calculator::get_max_priority(size_t size)
{
	size_t max_priority = 3; // наивысший приоритет ^

	for (size_t i = 0; i < size; i++)
	{
		max_priority = (priorities[i] > max_priority ? priorities[i] : max_priority);
	}

	return max_priority;
}

/* Получаем максимальное количество подряд идущих степеней в строке, из тех, что еще не считаны */
size_t Calculator::get_max_pow(const char* str, size_t shift)
{
	size_t max_pow_left = 0; // кол-во степеней слева от позиции уже считанных знаков
	size_t max_pow_right = 0; // кол-во степеней справа от позиции уже считанных знаков
	// для рассчетов
	size_t temp_pow_left = 0; 
	size_t temp_pow_right = 0;

	char last_sign = NULL; // последний знак
	int i = 0; // кол-во знаков всего
	while (*str)
	{
		if (*str == '+' || *str == '/' || *str == '*')
		{
			i++;
			last_sign = *str;
		}
		else if (((int) * (str - 1) >= 48 && (int)*(str - 1) <= 57) && *str == '-')
		{
			i++;
			last_sign = *str;
		}
		
		if (*str == '^')
		{
			i++;
			
			// если прошлый знак не был возведением в степень
			if (last_sign != '^')
			{
				//  забираем максимумы и очищаем кол-во подряд идущих знаков
				max_pow_left = (temp_pow_left > max_pow_left ? temp_pow_left : max_pow_left);
				max_pow_right = (temp_pow_right > max_pow_right ? temp_pow_right : max_pow_right);
				temp_pow_left = 0;
				temp_pow_right = 0;
			}

			//определяем к какому из счетчиков прибавлять значение
			if (i <= shift)
			{
				temp_pow_left++;
			}
			else
			{
				temp_pow_right++;
			}
			last_sign = *str;
		}
		++str;
	}

	max_pow_left = (temp_pow_left > max_pow_left ? temp_pow_left : max_pow_left);
	max_pow_right = (temp_pow_right > max_pow_right ? temp_pow_right : max_pow_right);

	// если подряд идущий степеней в левой части от считанных знаков было больше,
	// чем в правой, то нам это не важно и мы возвращаем 0. 
	return (max_pow_left >= max_pow_right ? 0: max_pow_right);
}

/* Считаем действия из выражения */
size_t Calculator::read_operators(const char* str)
{
	size_t i = 0; // счетчик для массивов знаков действий и приоритетов действий
	size_t coef = 0; // коэффициент для приортета действий
	size_t coef_shift = 0; // шаг для коэффициента
	size_t coef_pow = 0; // коэффициент для приортета действий подряд идущих степеней
	// для расчета степеней
	char* str_temp;
	memcpy(&str_temp, &str, sizeof(str));

	while (*str)
	{
		switch (*str)
		{
		case '+':
			operators[i] = *str;
			priorities[i++] = 1 + coef;
			break;
		case '*':
			operators[i] = *str;
			priorities[i++] = 2 + coef;
			break;
		case '/':
			operators[i] = *str;
			priorities[i++] = 2 + coef;
			break;
		case '^':
			operators[i] = *str;
			if (i > 0) 
			{
				if (operators[i - 1] == '^')
				{
					coef_pow++;
				}
				else
				{
					coef_pow = 0;
				}
			}
			priorities[i++] = 3 + coef+coef_pow;
			break;
		case '-':
			// если до минуса есть число или ) -> это не знак отрицательного числа
			if (((int)*(str - 1) >= 48 && (int)*(str - 1) <= 57)|| *(str - 1) == ')' || *(str - 1) == '.')
			{
				operators[i] = *str;
				priorities[i++] = 1 + coef;
			}
			break;
		case '(':
			// рассчитываем коэффициент через процедуру только 1 раз
			if (coef_shift == 0)
			{
				// максимальный имеющийся приоритет до первой скобки+максимальное кол-во подряд идущих степеней+1
				coef_shift = get_max_priority(i)+ get_max_pow(str_temp, i-1)+1;
			}
			coef += coef_shift;
			break;
		case ')':
			coef -= coef_shift;
			break;
		}

		++str;
	}

	// возвращаем размер массива
	return i;
}

/* Ищем индекс максимального приоритета действия */
size_t Calculator::get_index_max_priority(size_t size_priorities)
{
	size_t index = 0;

	for (size_t i = 0; i < size_priorities; i++)
	{
		index = (priorities[index] < priorities[i] ? i : index);
	}

	return index;
}

/* Производим рассчёт */
double Calculator::get_answer(size_t &size_digits, size_t &size_operators)
{
	// индекс выбранного оператора
	size_t oper;
	double new_val = digits[0];
	// пока у нас не останется одно число
	while (size_digits > 1)
	{
		// получаем индекс оператора наивысшего приоритета
		oper = get_index_max_priority(size_operators);

		// выполняем действия с числами 
		switch (operators[oper])
		{
		case '+':
			new_val = digits[oper] + digits[oper + 1];
			break;
		case '-':
			new_val = digits[oper] - digits[oper + 1];
			break;
		case '*':
			new_val = digits[oper] * digits[oper + 1];
			break;
		case '/':
			// проверка ошибки деления на 0
			if (digits[oper + 1] == 0)
			{
				throw runtime_error(error_div_zero);
			}

			new_val = digits[oper] / digits[oper + 1];
			break;
		case '^':
			new_val = pow(digits[oper], digits[oper + 1]);
			break;
		}
		//cout << digits[oper] << operators[oper] << digits[oper + 1] << "=" << new_val << " приоритет " << priorities[oper] << endl;

		// убираем рассчитанные значения из массивов и размеров массивов 
		memory_reallocation(size_operators, oper, new_val);
		size_digits--;
		size_operators--;
	}
	return new_val;
}

/* Изменяем массивы приоритетов, операторов и чисел */
void Calculator::memory_reallocation(size_t size, size_t index, double new_val)
{
	char *temp_operators = (char*)malloc(size-1);
	int  *temp_priorities = (int*)malloc((size-1) * sizeof(int));
	double *temp_digits = (double*)malloc((size) * sizeof(double));

	if (temp_operators == NULL || temp_priorities == NULL || temp_digits == NULL)
	{
		throw runtime_error(error_memory);
	}

	for (size_t i = 0; i < size+1; i++)
	{
		if (i < index)
		{
			temp_operators[i] = operators[i];
			temp_priorities[i] = priorities[i];
			temp_digits[i] = digits[i];
		}
		else if(i > index)
		{
			if (i < size)
			{
				temp_operators[i - 1] = operators[i];
				temp_priorities[i - 1] = priorities[i];
			}

			if (i >= index + 2)
			{
				temp_digits[i-1] = digits[i];
			}
		}
		else if (i == index)
		{
			temp_digits[i] = new_val;
		}
	}

	// в этом случае пусть освобождает деструктор для массивов операторов и приоритетов
	if (size != 1) 
	{
		free(operators);
		free(priorities);

		operators = (char*)malloc((size - 1));
		priorities = (int*)malloc((size - 1) * sizeof(int));

		if (operators == NULL || priorities == NULL)
		{
			throw runtime_error(error_memory);
		}

		memcpy(operators, temp_operators, (size-1));
		memcpy(priorities, temp_priorities, ((size-1) * sizeof(int)));
	}

	free(digits);
	digits = (double*)malloc(size * sizeof(double));

	if (digits == NULL )
	{
		throw runtime_error(error_memory);
	}

	memcpy(digits, temp_digits,(size * sizeof(double)));

	free(temp_digits);
	free(temp_operators);
	free(temp_priorities);
}

/* Очищаем массивы */
void Calculator::clear_data()
{
	free(digits);
	free(operators);
	free(priorities);
}
