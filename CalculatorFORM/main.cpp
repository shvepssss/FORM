#include "Calculator.h"

const char* mes = "Введите F для выхода из программы или выражение для рассчёта:";

int main()
{
	setlocale(LC_ALL, "Russian");

	// выделили память под строку
	const int len = 1000;
	double answer;
	char* str = (char*)malloc(len);
	// проверили, что свободная память есть и ее удалось выделить
	if (str == NULL)
	{
		exit(1);
	}
	cout << mes << endl;

	while (cin.get(str, len))
	{
		
		// условие выхода из цикла
		if (str[0] == 'F')
		{
			break;
		}

		// выполняем расчёт
		Calculator* calculator = (Calculator*)malloc(sizeof(Calculator));

		if (calculator == NULL)
		{
			exit(1);
		}

		try
		{
			answer = calculator->calc(str);
			cout << "Ответ:" << answer << endl;
		}
		catch (const runtime_error& e)
		{
			cout << "Ошибка: " << e.what() << endl;
		}

		// подготовка к считыванию новой строки
		calculator->~Calculator();
		free(calculator);
		free(str);
		str = (char*)malloc(len);
		// проверили, что свободная память есть и ее удалось выделить
		if (str == NULL)
		{
			exit(1);
		}

		// чтобы продолжать считывать строки
		cin.ignore();
		cout << mes << endl;
	}
	free(str);

	return 0;
}