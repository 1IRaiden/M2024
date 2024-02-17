#include "../variables.h"

// Функция считает повышение значения на к/л процент.
float percentage_calculation(float value, int time_unit){
    float sum = 0;
    for (int i=0; i<time_unit; i++){
        value = value * (1 + INFLATION);
        sum = sum + value;
    }
    return sum;
}

float entire_period_expenses(float expenses){
    // m_* it means "monthly"
    // Считаю сколько потратит денег за n лет
    float entire_period_expenses = percentage_calculation(expenses * 12, YEARS);
    return entire_period_expenses;
}

float entire_period_deposit_balance(){
    float margin = (SALARY - WASTES - FLAT_RENT) * 12;
    float balance = 0;
    for (int i=0; i<YEARS; i++){
        /* Считает годовую маржу и умножает на инфляцию.
        '1' в аргументе - значит проходит один цикл в функции entire_period_expenses(),
        т.к. 30-годовой цикл выполняется внутри текущей функции, для того чтобы текущую годовую
        маржу прибавлять к депозиту и умножать на процент по вкладу.*/ 
        margin = percentage_calculation(margin, 1);
        balance = balance + margin;
        balance = percentage_calculation(balance, 1);
    }
    return balance;
}