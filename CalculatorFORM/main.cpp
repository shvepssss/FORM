#include "Calculator.h"

const char* mes = "������� F ��� ������ �� ��������� ��� ��������� ��� ��������:";

int main()
{
	setlocale(LC_ALL, "Russian");

	// �������� ������ ��� ������
	const int len = 1000;
	double answer;
	char* str = (char*)malloc(len);
	// ���������, ��� ��������� ������ ���� � �� ������� ��������
	if (str == NULL)
	{
		exit(1);
	}
	cout << mes << endl;

	while (cin.get(str, len))
	{
		
		// ������� ������ �� �����
		if (str[0] == 'F')
		{
			break;
		}

		// ��������� ������
		Calculator* calculator = (Calculator*)malloc(sizeof(Calculator));

		if (calculator == NULL)
		{
			exit(1);
		}

		try
		{
			answer = calculator->calc(str);
			cout << "�����:" << answer << endl;
		}
		catch (const runtime_error& e)
		{
			cout << "������: " << e.what() << endl;
		}

		// ���������� � ���������� ����� ������
		calculator->~Calculator();
		free(calculator);
		free(str);
		str = (char*)malloc(len);
		// ���������, ��� ��������� ������ ���� � �� ������� ��������
		if (str == NULL)
		{
			exit(1);
		}

		// ����� ���������� ��������� ������
		cin.ignore();
		cout << mes << endl;
	}
	free(str);

	return 0;
}