#include "physicsengine.h"
#include "controlpanelwidget.h"
#include <cmath>

// Конструктор
PhysicsEngine::PhysicsEngine(QObject *parent)
    : QObject(parent)
{
    m_elapsedSec = 0; //общее время симуляции в секундах.
    m_state = ExperimentState::Idle;
    m_pointCount = 0;

    m_timer = new QTimer(this);
    m_envTemp = 20.0;
    connect(m_timer, &QTimer::timeout, this, &PhysicsEngine::onTimerTick);
}

// Настройка параметров перед стартом
void PhysicsEngine::configure(double envTemp, const QVector<Sample>& samples)
{
    // TODO: Сохрани температуру среды (m_envTemp)
    // TODO: Сохрани копию образцов (m_samples)
    // TODO: Сбрось счётчики времени и точек в ноль

    m_envTemp = envTemp;
    m_samples = samples;

    m_pointCount = 0;
    m_elapsedSec = 0;
    m_coolingStartSec = 0;
}

// Запуск нагрева
void PhysicsEngine::startHeating()
{
    // TODO: Проверка: если состояние не Idle, выйти (return)
    // TODO: Переключи состояние на Heating
    // TODO: Отправь сигнал stateChanged
    // TODO: Запусти таймер (например, старт с интервалом 500 мс)

    if(m_state != ExperimentState::Idle) return;
    m_state = ExperimentState::Heating;
    emit stateChanged(m_state);
    m_timer->start(500);
}

// Запуск термостатирования
void PhysicsEngine::startThermostatting()
{
    // TODO: Проверка: если состояние не Heating, выйти
    // TODO: Переключи состояние на Thermostatting
    // TODO: Отправь сигнал stateChanged

    if(m_state != ExperimentState::Heating) return;
    m_state = ExperimentState::Thermostate;
    emit stateChanged(m_state);
}

// Запись точки (и переход к остыванию)
void PhysicsEngine::recordPoint()
{
    // TODO: Если сейчас Thermostatting:
    //       1. Переключи состояние на Cooling
    //       2. Запомни текущее время как начало остывания (m_coolingStartSec)
    //       3. Отправь сигнал stateChanged
    if(m_state == ExperimentState::Thermostate){
        m_state = ExperimentState::Cooling;
        m_coolingStartSec = m_elapsedSec;
        emit stateChanged(m_state);
    }
    // TODO: Если сейчас Cooling:
    //       1. Посчитай время остывания (текущее время - время начала остывания)
    //       2. Собери текущие температуры образцов в вектор
    //       3. Отправь сигнал pointRecorded (номер точки, время, температуры)
    //       4. Увеличь счётчик точек

    if(m_state == ExperimentState::Cooling){
        QVector<double> temps;
        double coolingTimeSec;
        for(int i = 0; i < m_samples.size(); i++){
            temps.append(m_samples[i].currentTemp);
        }
        coolingTimeSec = m_elapsedSec - m_coolingStartSec;
        m_pointCount++;
        emit pointRecorded(m_pointCount, coolingTimeSec, temps);
    }
}

// Полный сброс
void PhysicsEngine::reset()
{
    // TODO: Останови таймер
    // TODO: Сбрось состояние на Idle
    // TODO: Обнули все счётчики
    // TODO: Верни температуры всех образцов к температуре среды (m_envTemp)
    // TODO: Отправь сигналы stateChanged и temperaturesUpdated
    m_timer->stop();
    m_state = ExperimentState::Idle;
    emit stateChanged(m_state);

    m_elapsedSec = 0;
    m_coolingStartSec = 0;
    m_pointCount = 0;

    for(int i = 0; i < m_samples.size(); i++){
        m_samples[i].currentTemp = m_envTemp;
    }
    QVector<double> temps;
    for (int i = 0; i < m_samples.size(); i++){
        temps.append(m_samples[i].currentTemp);
    }
    emit temperaturesUpdated(temps, 0);
}

// Основной цикл физики (вызывается таймером)
void PhysicsEngine::onTimerTick()
{
    // TODO: Увеличь виртуальное время (m_elapsedSec += 0.5)
    m_elapsedSec += 0.5;

    bool tempsChanged = false;

    switch (m_state) {
    case ExperimentState::Heating: {
        // TODO: Для каждого активного образца:
        //       Если T < 60.0, увеличь T (например, на 0.5 градуса за тик)
        //       Если T > 60.0, зафиксируй на 60.0
        for(int i = 0; i < m_samples.size(); i++){
            if(m_samples[i].isActive){
                if(m_samples[i].currentTemp < 60){
                    m_samples[i].currentTemp += 0.5;
                    if(m_samples[i].currentTemp > 60) m_samples[i].currentTemp = 60;
                    tempsChanged = true;
                }
            }
        }

        // TODO: Проверь, нагрелись ли ВСЕ активные образцы до 60.0
        //       Если да -> переключи на Thermostatting и отправь сигнал
        bool allHot = true;
        for(int i = 0; i < m_samples.size(); i++){
            if(m_samples[i].isActive){
                if(m_samples[i].currentTemp < 60){
                    allHot = false;
                    break;
                }
            }
        }
        if(allHot){
            m_state = ExperimentState::Thermostate;
            emit stateChanged(m_state);
        }
    }

    case ExperimentState::Thermostate: {
        // TODO: Для каждого активного образца жестко установи T = 60.0
        for(int i = 0; i < m_samples.size(); i++){
            if(m_samples[i].isActive){
                    m_samples[i].currentTemp = 60;
                    tempsChanged = true;
                }
            }
        break;
    }
    case ExperimentState::Cooling: {
        // TODO: Посчитай время остывания в МИНУТАХ (важно для формулы!)
        // TODO: Для каждого активного образца:
        //       1. Получи теплоемкость C_ob через метод calculateHeatCapacity()
        //       2. Посчитай показатель экспоненты по формуле (3.5) из методички
        //          (используй константы K_CONST и C_K)
        //       3. Обнови текущую температуру по формуле экспоненциального затухания
        double t_min = (m_elapsedSec - m_coolingStartSec) / 60;

        for(int i = 0; i < m_samples.size(); i++){
            if(m_samples[i].isActive){
                double c_ob = m_samples[i].calculateHeatCapacity();
                double denominator = c_ob + C_K;
                if(denominator <= 0) continue;
                double exponent = -(K_CONST * t_min) / denominator;
                double newTemp = m_envTemp + (60 - m_envTemp) * std::exp(exponent);
                m_samples[i].currentTemp = newTemp;
                tempsChanged = true;
            }
        }
        tempsChanged = true;
        break;
    }
    default:
        break;
    }

    // TODO: Если температуры изменились или мы в режиме термостатирования:
    //       Собери все текущие температуры в вектор и отправь сигнал temperaturesUpdated
    // После закрывающей скобки switch(m_state):


    if(tempsChanged == true || m_state == ExperimentState::Thermostate){
        QVector<double> temps;
        for(int i = 0; i < m_samples.size(); i++){
            temps.append(m_samples[i].currentTemp);
        }
        emit temperaturesUpdated(temps, m_elapsedSec);
    }
}
