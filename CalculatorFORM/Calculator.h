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
	// ������ �������� ��������� 
	double calc(const char* str) throw(runtime_error);

private:
	// �������� ������ ��� ��������
	void get_str_ws(const char* str, char* str_ws);
	// �������� ������ � ���������
	bool has_errors(const char* str);
	// �������� ������� . � �����
	int check_point(const char* str, int size);
	// �������������� ������� �������� � �����
	double str_to_double(const char* str, int size);
	// ��������� ����� �� ���������
	size_t read_digits(const char* str);
	// �������� ������������ ��������� ��������
	size_t get_max_priority(size_t size);
	// �������� ������������ ���������� ������ ������ �������� � ������, �� ���, ��� ��� �� �������
	size_t get_max_pow(const char* str, size_t shift);
	// ������� �������� �� ���������
	size_t read_operators(const char* str);
	// ���� ������ ������������� ���������� ��������
	size_t get_index_max_priority(size_t size);
	// ���������� �������
	double get_answer(size_t &size_digits, size_t &size_operators);
	// �������� ������� �����������, ���������� � �����
	void memory_reallocation(size_t size, size_t index, double new_val);
	// ������� �������
	void clear_data();

	// �����
	double *digits;
	// ��������� (�������� � ���������)
	char *operators;
	// ���������� ����������
	int *priorities;
};

