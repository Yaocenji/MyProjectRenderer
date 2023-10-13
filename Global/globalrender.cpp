#include "globalrender.h"

globalrender::globalrender() {
}

QColor globalrender::backgroundColor = QColor(1, 1, 1);

bool globalrender::real_time_render = true;
