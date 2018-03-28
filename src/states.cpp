#include "states.h"
#include "mysimulator.h"

States::States(QObject *parent) : QObject(parent),
    m_crashed(new QState()),
    m_idle(new QState()),
    m_finished(new QState()),
    m_parsing(new QState()),
    m_continued(new QState()),
    m_paused(new QState()),
    m_reset(new QState()),
    m_unPaused(new QHistoryState()),
    m_machine(new QStateMachine(this))
{
    m_crashed->setObjectName("Crashed");
    m_idle->setObjectName("Idle");
    m_finished->setObjectName("Finished");
    m_parsing->setObjectName("Parsing");
    m_continued->setObjectName("Continued");
    m_paused->setObjectName("Paused");
    m_reset->setObjectName("Reset");
    m_unPaused->setObjectName("Unpaused");
    m_list << QVariant::fromValue(m_idle) << QVariant::fromValue(m_crashed) << QVariant::fromValue(m_finished) << QVariant::fromValue(m_parsing) << QVariant::fromValue(m_continued) << QVariant::fromValue(m_paused) << QVariant::fromValue(m_reset);
}

void States::setupStates(AtomifySimulator &simulator)
{
    m_machine->addState(m_parsing);
    m_machine->addState(m_crashed);
    m_machine->addState(m_idle);
    m_machine->addState(m_finished);
    m_machine->addState(m_continued);
    m_machine->addState(m_paused);
    m_machine->addState(m_reset);
    m_machine->addState(m_unPaused);

//    m_idle->addTransition(&simulator, SIGNAL(started()), m_parsing);

//    m_parsing->addTransition(&simulator, SIGNAL(crashed()), m_crashed);
//    m_parsing->addTransition(&simulator, SIGNAL(finished()), m_finished);
//    m_parsing->addTransition(&simulator, SIGNAL(paused()), m_paused);
//    m_parsing->addTransition(&simulator, SIGNAL(reset()), m_reset);

//    m_crashed->addTransition(&simulator, SIGNAL(reset()), m_reset);

//    m_finished->addTransition(&simulator, SIGNAL(continued()), m_continued);
//    m_finished->addTransition(&simulator, SIGNAL(reset()), m_reset);
//    m_finished->addTransition(&simulator, SIGNAL(parsing()), m_parsing);

//    m_continued->addTransition(&simulator, SIGNAL(reset()), m_reset);
//    m_continued->addTransition(&simulator, SIGNAL(paused()), m_paused);
//    m_continued->addTransition(&simulator, SIGNAL(parsing()), m_parsing);

//    m_paused->addTransition(&simulator, SIGNAL(unPaused()), m_unPaused);
//    m_paused->addTransition(&simulator, SIGNAL(reset()), m_reset);

//    m_reset->addTransition(&simulator, SIGNAL(didReset()), m_idle);
//    m_continued->addTransition(&simulator, SIGNAL(crashed()), m_crashed);

//    m_machine->setInitialState(m_idle);

//    connect(&simulator, &AtomifySimulator::paused, this, [&]() {
//        if(m_parsing->active()) {
//            m_unPaused->setDefaultState(m_parsing);
//        } else if(m_continued->active()) {
//            m_unPaused->setDefaultState(m_continued);
//        }
//    });

//    m_machine->start();
}

QString States::currentStateString()
{
    if(m_parsing->active()) {
        return "parsing";
    }
    if(m_crashed->active()) {
        return "crashed";
    }
    if(m_idle->active()) {
        return "idle";
    }
    if(m_finished->active()) {
        return "finished";
    }
    if(m_continued->active()) {
        return "continued";
    }
    if(m_paused->active()) {
        return "paused";
    }
    if(m_reset->active()) {
        return "reset";
    }
    if(m_unPaused->active()) {
        return "unPaused";
    }
}

QState *States::reset() const
{
    return m_reset;
}

QState *States::crashed() const
{
    return m_crashed;
}

QState *States::idle() const
{
    return m_idle;
}

QState *States::finished() const
{
    return m_finished;
}

QState *States::parsing() const
{
    return m_parsing;
}

QVariantList States::list() const
{
    return m_list;
}

QState *States::continued() const
{
    return m_continued;
}

QState *States::paused() const
{
    return m_paused;
}

QHistoryState *States::unPaused() const
{
    return m_unPaused;
}

QStateMachine *States::machine() const
{
    return m_machine;
}


void States::setCrashed(QState *crashed)
{
    if (m_crashed == crashed)
        return;

    m_crashed = crashed;
    emit crashedChanged(crashed);
}

void States::setIdle(QState *idle)
{
    if (m_idle == idle)
        return;

    m_idle = idle;
    emit idleChanged(idle);
}

void States::setFinished(QState *finished)
{
    if (m_finished == finished)
        return;

    m_finished = finished;
    emit finishedChanged(finished);
}

void States::setParsing(QState *parsing)
{
    if (m_parsing == parsing)
        return;

    m_parsing = parsing;
    emit parsingChanged(parsing);
}

void States::setList(QVariantList list)
{
    if (m_list == list)
        return;

    m_list = list;
    emit listChanged(list);
}

void States::setContinued(QState *continued)
{
    if (m_continued == continued)
        return;

    m_continued = continued;
    emit continuedChanged(continued);
}

void States::setPaused(QState *paused)
{
    if (m_paused == paused)
        return;

    m_paused = paused;
    emit pausedChanged(paused);
}

void States::setUnPaused(QHistoryState *unPaused)
{
    if (m_unPaused == unPaused)
        return;

    m_unPaused = unPaused;
    emit unPausedChanged(unPaused);
}

void States::setMachine(QStateMachine *machine)
{
    if (m_machine == machine)
        return;

    m_machine = machine;
    emit machineChanged(machine);
}

void States::setReset(QState *reset)
{
    if (m_reset == reset)
        return;

    m_reset = reset;
    emit resetChanged(reset);
}

