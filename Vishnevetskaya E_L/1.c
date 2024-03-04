#include <stdio.h>
#include <stdlib.h>

// ���������, ����������� ��������
struct Person {
    float salary;           // ��������
    float savings;          // ����������
    float expenses;         // �������
    float deposit;          // �����
    float ipoteka_rate;    // ���������� ������ �������
    float deposit_rate;     // ���������� ������ ������
    float ipoteka_dolg;
};
double month_inflation_rate = 0.12/12;
float apartment_price = 20000000;

// ������� �������� � �������������� ��� ������
struct Person Alice = {.salary=300000,.savings=2000000,.expenses=120000,.ipoteka_rate=0.18,.ipoteka_dolg=20000000};
struct Person Bob = {.salary=300000,.expenses=45000,.deposit=2000000,.deposit_rate=0.16};

// ������� ��� ������ ������� ������
void payIpotekaVznos() {
    Alice.ipoteka_dolg -= Alice.savings;
    Alice.savings = 0;
}
// ������� ��� ������ �������
void payIpoteka() {
    Alice.savings = Alice.salary - Alice.expenses;
    Alice.ipoteka_dolg -= Alice.savings;
    Alice.ipoteka_dolg *= 1 + Alice.ipoteka_rate/12;
    Alice.savings = 0;
}

// ������� ��� ������ �������
void AliceSavings() {
    Alice.savings+=abs(Alice.ipoteka_dolg);
    Alice.ipoteka_dolg = 0;
    Alice.savings += Alice.salary - Alice.expenses;
}

// ������� ��� �������������� ����� �� �����
void investDeposit() {
   Bob.savings = Bob.salary - Bob.expenses;
   Bob.deposit += Bob.savings;
   Bob.deposit *= 1 + Bob.deposit_rate/12;
   Bob.savings = 0;
}

void inflation(){
    Alice.expenses *= (1 + month_inflation_rate);
    Alice.salary *= (1 + month_inflation_rate);
    Bob.expenses *= (1 + month_inflation_rate);
    Bob.salary *= (1 + month_inflation_rate);
    apartment_price *= (1 + month_inflation_rate);
    
}

void SomeBad(int month,int year, int bad_month, int bad_year){
    if (month == bad_month && year == bad_year)
    {
        
    }
}

// ������� 
void Simulation(int start_year, int end_year) {
    //������ ������ ������ �� �������
    payIpotekaVznos();

    // ���� �� ����� � �������
    for (int year = start_year; year <= end_year; ++year) {
        for (int month = 1; month <= 12; ++month) {
            //SomeBad(month,year,9,2045)
            if (Alice.ipoteka_dolg>0){
                payIpoteka();
            }
            else{
                AliceSavings();
            }

            investDeposit();
            inflation();
        }
    }
    // ��� ����� ��������
    Bob.savings = Bob.deposit- apartment_price;
}

int main() {

    Simulation(2024,2054);
    // ����� ���������� � ��������� ��������
    printf("Alice money ost: %.2f rub\n", Alice.savings);
    printf("Bob money ost: %.2f rub\n", Bob.savings);
    
    return 0;
}
