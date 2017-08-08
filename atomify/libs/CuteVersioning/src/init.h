#ifndef INIT_H
#define INIT_H

#include "./quickversion.h"

#include <QtQml>

namespace CuteVersioning {

void init() {
    qmlRegisterSingletonType<QuickVersion>("CuteVersioning", 1, 0, "Version", &QuickVersion::qmlInstance);
}

}

#endif // INIT_H
