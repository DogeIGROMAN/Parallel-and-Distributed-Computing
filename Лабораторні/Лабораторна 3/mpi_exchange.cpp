//Розпарарелелювання
#include <stdio.h> 
#include "mpi.h" 
#include <stdlib.h> 
#include <iostream>
#include <math.h>
//Запуск через mpiexec -n 1 "MPI project" в консолі та папці файла exe проекту 
//функція факторіала
long long Factorial(int n)
{
	long long factorial = 1;
	for (long long i = 1;i <= n;i++)
		factorial=factorial * i;
	return factorial;
}

int main(int argc, char* argv[])
{
	int my_rank; // номер процесу (0,1,2,...)
	int numprocs; // кількість процесів
	int n;
	long double sum;
	int a;
	long double result;
	double wtime;
	//Ініціалізація MPI
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);//записуємо номер процеса в MPI
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);//запис кількості процесі в MPI

	//Перший, тобто головний процес 0, приймає значення n від користувача
	if (my_rank == 0) 
	{
		printf("Input n:");
		fflush(stdout);
		scanf_s("%d", &n);
	}
	//Береться n, 
	// 1 це тип елемента відпраляє, 
	// MPI_INT вже сам тип який відправляє, 
	// процес 0 відправляє цей n, 
	// MPI_COMM_WORLD - відправляє усім процесам ці дані тобто це звязок.
	MPI_Bcast(
		&n,
		1, 
		MPI_INT, 
		0, 
		MPI_COMM_WORLD);
	//Синхронізація усіх процесів в MPI_COMM_WORLD
	MPI_Barrier(MPI_COMM_WORLD);
	//запис початку вимірювання часу
	wtime = MPI_Wtime();
	
	//Робота розпаралелювання
	
	//Ініциалізація початкової змінної суми
	sum = 0;
	//Кожен процес i до кількості завдання n, буде обробляти сумування рандомного a.
	//Тобто кожен цикл задачі сумування буде обробляти кожен процес окремо для оптимізації
	for (int i = my_rank; i <= n; i += numprocs)
	{
		long long fac = Factorial(i);
		sum += 1.0 / fac;
	}
	//Передача даних від усіх процесів одному
	//Бере вже обчсилені суми sum з інших процесів, 
	// в змінну result результа, 1 це тип елемента відпраляє, 
	// MPI_INT той самий один тип, 
	// MPI_SUM операція додавання усіх sum в MPI,
	// приймає дані від усіх процесів - процес 0 в змінну result попередньо оброблені,
	// MPI_COMM_WORLD - звязок з усіма процесами
	MPI_Reduce(
		&sum,
		&result, 
		1, 
		MPI_LONG_DOUBLE,
		MPI_SUM, 
		0,
		MPI_COMM_WORLD);
	//Завершення вимірювання часу і запис пройденого часу в wtime
	MPI_Barrier(MPI_COMM_WORLD);
	wtime = MPI_Wtime() - wtime;

	//В головному процесі виводимо отримані дані
	if (my_rank == 0) 
	{
		printf("\nsuma: %.9lf\n", result);
		printf("\nWorking time: %.8lf seconds\n", wtime);

	}
	//Завершення MPI
	MPI_Finalize();
	return 0;
}
