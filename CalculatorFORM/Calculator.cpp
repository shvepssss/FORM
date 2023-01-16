#include "Calculator.h"

const int len = 500;

const char* error_content = "��������� �������� ������, ����������� �� ����� ��� ���������.";
const char* error_div_zero = "��������� �������� ������� ����� �� 0, ������� ����������.";
const char* error_memory = "��������� �� ����� �������� ������ �������.";


/* ���������� */
Calculator::~Calculator()
{
	// ����������� ������
	clear_data();
}

/* ������ �������� ��������� */
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

	// ���������� ������ ��� ��������
    get_str_ws(str, str_ws);

	// ��������� ��������� �� ������
	if (has_errors(str_ws))
	{
		throw runtime_error(error_content);
	}

	// ��������� �����
	size_t size_digits = read_digits(str_ws);
	// ��������� ����� ��������
	size_t size_operators = read_operators(str_ws);

	// ��������, ��� ���������� �������� ������ �� 1, ��� ����, ����� � ������ ��������� ������
	if (size_operators != size_digits - 1)
	{
		throw runtime_error(error_content);
	}

	// ���������� �������
	double answer = get_answer(size_digits, size_operators);
	free(str_ws);

	return answer;
}

/* �������� ������ ��� �������� */
void Calculator::get_str_ws(const char* str, char* str_ws)
{
	size_t i = 0;
	// ��������� ������ -�� ������
	char last_val=' ';
	bool space = false;

	while (*str)
	{
		if (*str != ' ')
		{
			*str_ws = *str;
			++str_ws;

			// ������������ "8 8"
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

	// ������������ � ������� �������
	while (i > 0)
	{
		--str_ws;
		--i;
	}
}

/* �������� ������ � ���������. true - ������ ����, false - ������ ��� */
bool Calculator::has_errors(const char* str)
{
	bool answer = false;
	size_t i=0, count_lbrackets = 0, count_rbrackets = 0, count_points = 0;

	// --5
	if (str[0] == '-' && str[1] == '-') 
		answer = true;

	while (*str)
	{
		// �������� ���-�� ( � )
		if (*str == '(')
		{
			// ������������ ������ 8(
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
			// ������������ ������ )8
			if (((int)*(str + 1) >= 48 && (int)*(str + 1) <= 57) || *(str + 1) == '.')
			{
				answer = true;
				break;
			}
			count_rbrackets++;
		}
		// �������� �� )(
		if (count_rbrackets > count_lbrackets)
		{
			answer = true;
			break;
		}
		
		// �������, ����� ���� � ������
		if (!(((int)*str >= 48 && (int)*str <= 57) || *str == '.' || *str == '-'
			|| *str == '+' || *str == '*' || *str == '/' || *str == '^' 
			|| *str == '(' || *str == ')'))
		{
			answer = true;
			break;
		}

		// ����� �������� ������ ����� *�����-�� ����*-
		if (i > 0)
		{
			if (( *str == '+' || *str == '*' || *str == '/' || *str == '^')
				&& (*(str-1) == '-' || *(str - 1) == '+' || *(str - 1) == '*' || *(str - 1) == '/' || *(str - 1) == '^'))
			{
				answer = true;
				break;
			}
		}

		// �������� �� 5.5.5 
		if (*str == '.')
		{
			count_points++;
		}
		if (( * str == '-' || *str == '+' || *str == '*' || *str == '/' || *str == '^' || *str == ')') && count_points>0)
		{
			count_points--;
		}
		// ���� ���-�� ������ ������ ����� ����� ������� ������ �����
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

/* �������� ������� . � ����� */
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

/* �������������� ������� �������� � ����� */
double Calculator::str_to_double(const char* str, int size)
{
	// ����������, ���� �� ����� � ������ �����
	int pozition_point = check_point(str, size);
	double digit= 0; // ���������� �����
	double coef = 1; // ����������� ��� ������� 

	// ����������� ����������� ��� ����������� ������� ����� ����� ������ �� �����
	if (pozition_point != -1)
	{
		for (size_t j = 0; j < size- pozition_point - 1; j++)
		{
			coef *= 0.1;
		}
	}

	// ����������� �����
	for (int i = size-1; i>=0;i--)
	{
		if ((int)str[i] >= 48 && (int)str[i] <= 57) // ���� ������ ��� �����
		{
			digit += (str[i]-48) * coef;
			coef *= 10;
		}
		else if (str[i] == '-' && i==0) // ���� ������ ��� ���� ���������
		{
			digit *= -1;
		}
	}

	return digit;
}

/* ��������� ����� �� ��������� */
size_t Calculator::read_digits(const char* str)
{
	char* digit_str; // ����������� �����
	digit_str = (char*)malloc(len);

	if (digit_str == NULL)
	{
		throw runtime_error(error_memory);
	}

	size_t i = 0; // ������� ��� ������� �����
	size_t j = 0; // ������� ��� �������-�������� �����

	while(*str)
	{
		// ����� . -
		if (((int)*str >= 48 && (int)*str <= 57) || *str == '.' || *str == '-')
		{
			// �������� ��������� ��� ��������� ��������
			if (*str == '.')
			{
				// ��������� �� ���������� ���� ����� �� .
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
				// ��������� �� ���������� ���� ������ �� .
				if (*(str + 1) == NULL || !((int)*(str + 1) >= 48 && (int)*(str + 1) <= 57))
				{
					digit_str[j++] = '0';
				}
			}
			else if(*str == '-')
			{
				// ���� ����� �����, ��� �� �����(� �� �����!) - �� ��� ������������� �����
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
		// ����� ��� ���� ��������, ���� (j==0 && i==0 - � ��� ����, �� ��� ������ ����� ���� � ������ ���������, ��� �������� �������
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

	// ��������� ��������� �����, ���� ��� ����
	if (j != 0)
	{
		digits[i] = str_to_double(digit_str, j);
		i++;
	}
	free(digit_str);
	j = 0;

	// ���������� ������ �������
	return i;
}

/* �������� ������������ ��������� �������� */
size_t Calculator::get_max_priority(size_t size)
{
	size_t max_priority = 3; // ��������� ��������� ^

	for (size_t i = 0; i < size; i++)
	{
		max_priority = (priorities[i] > max_priority ? priorities[i] : max_priority);
	}

	return max_priority;
}

/* �������� ������������ ���������� ������ ������ �������� � ������, �� ���, ��� ��� �� ������� */
size_t Calculator::get_max_pow(const char* str, size_t shift)
{
	size_t max_pow_left = 0; // ���-�� �������� ����� �� ������� ��� ��������� ������
	size_t max_pow_right = 0; // ���-�� �������� ������ �� ������� ��� ��������� ������
	// ��� ���������
	size_t temp_pow_left = 0; 
	size_t temp_pow_right = 0;

	char last_sign = NULL; // ��������� ����
	int i = 0; // ���-�� ������ �����
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
			
			// ���� ������� ���� �� ��� ����������� � �������
			if (last_sign != '^')
			{
				//  �������� ��������� � ������� ���-�� ������ ������ ������
				max_pow_left = (temp_pow_left > max_pow_left ? temp_pow_left : max_pow_left);
				max_pow_right = (temp_pow_right > max_pow_right ? temp_pow_right : max_pow_right);
				temp_pow_left = 0;
				temp_pow_right = 0;
			}

			//���������� � ������ �� ��������� ���������� ��������
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

	// ���� ������ ������ �������� � ����� ����� �� ��������� ������ ���� ������,
	// ��� � ������, �� ��� ��� �� ����� � �� ���������� 0. 
	return (max_pow_left >= max_pow_right ? 0: max_pow_right);
}

/* ������� �������� �� ��������� */
size_t Calculator::read_operators(const char* str)
{
	size_t i = 0; // ������� ��� �������� ������ �������� � ����������� ��������
	size_t coef = 0; // ����������� ��� ��������� ��������
	size_t coef_shift = 0; // ��� ��� ������������
	size_t coef_pow = 0; // ����������� ��� ��������� �������� ������ ������ ��������
	// ��� ������� ��������
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
			// ���� �� ������ ���� ����� ��� ) -> ��� �� ���� �������������� �����
			if (((int)*(str - 1) >= 48 && (int)*(str - 1) <= 57)|| *(str - 1) == ')' || *(str - 1) == '.')
			{
				operators[i] = *str;
				priorities[i++] = 1 + coef;
			}
			break;
		case '(':
			// ������������ ����������� ����� ��������� ������ 1 ���
			if (coef_shift == 0)
			{
				// ������������ ��������� ��������� �� ������ ������+������������ ���-�� ������ ������ ��������+1
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

	// ���������� ������ �������
	return i;
}

/* ���� ������ ������������� ���������� �������� */
size_t Calculator::get_index_max_priority(size_t size_priorities)
{
	size_t index = 0;

	for (size_t i = 0; i < size_priorities; i++)
	{
		index = (priorities[index] < priorities[i] ? i : index);
	}

	return index;
}

/* ���������� ������� */
double Calculator::get_answer(size_t &size_digits, size_t &size_operators)
{
	// ������ ���������� ���������
	size_t oper;
	double new_val = digits[0];
	// ���� � ��� �� ��������� ���� �����
	while (size_digits > 1)
	{
		// �������� ������ ��������� ���������� ����������
		oper = get_index_max_priority(size_operators);

		// ��������� �������� � ������� 
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
			// �������� ������ ������� �� 0
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
		//cout << digits[oper] << operators[oper] << digits[oper + 1] << "=" << new_val << " ��������� " << priorities[oper] << endl;

		// ������� ������������ �������� �� �������� � �������� �������� 
		memory_reallocation(size_operators, oper, new_val);
		size_digits--;
		size_operators--;
	}
	return new_val;
}

/* �������� ������� �����������, ���������� � ����� */
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

	// � ���� ������ ����� ����������� ���������� ��� �������� ���������� � �����������
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

/* ������� ������� */
void Calculator::clear_data()
{
	free(digits);
	free(operators);
	free(priorities);
}
