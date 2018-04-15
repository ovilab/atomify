#include "LammpsWrappers/modifiers/modifiers.h"
#include "core/controllers/lammpscontroller.h"
#include "core/editor/codeeditorbackend.h"
#include "core/editor/highlighter.h"
#include "core/editor/linenumbers.h"
#include "core/visualization/atomify.h"
#include "dataproviders/data1d.h"
#include "dataproviders/data2d.h"
#include "helpers/clipboard.h"
#include "helpers/keysequence.h"
#include "helpers/mousemover.h"
#include "helpers/rendercapturehelper.h"
#include "helpers/standardpaths.h"
#include "helpers/windowgl2.h"
#include "states.h"
#include "usagestatistics.h"
using namespace atomify;

void registerQML()
{
    qmlRegisterType<Data1D>("Atomify", 1, 0, "Data1D");
    qmlRegisterType<Data2D>("Atomify", 1, 0, "Data2D");

    qmlRegisterType<LineNumbers>("Atomify", 1, 0, "LineNumbers");
    qmlRegisterType<CodeEditorBackend>("Atomify", 1, 0, "CodeEditorBackend");
    qmlRegisterType<HighlighterWrapper>("Atomify", 1, 0, "Highlighter");
    qmlRegisterType<WindowGL2>("Atomify", 1, 0, "WindowGL2");
    qmlRegisterType<MouseMover>("Atomify", 1, 0, "MouseMover");
    qmlRegisterType<States>("Atomify", 1, 0, "States");
    qmlRegisterType<UsageStatistics>("Atomify", 1, 0, "UsageStatistics");
    qmlRegisterType<Clipboard>("Atomify", 1, 0, "Clipboard");

    qmlRegisterSingletonType<StandardPaths>("StandardPaths", 1, 0, "StandardPaths", &StandardPaths::qmlInstance);

    qmlRegisterSingletonType<KeySequence>("KeySequence", 1, 0, "KeySequence", &KeySequence::qmlInstance);

    qmlRegisterSingletonType<RenderCaptureHelper>("Atomify", 1, 0, "RenderCaptureHelper", &RenderCaptureHelper::qmlInstance);

    qmlRegisterType<Atomify>("Atomify", 1, 0, "Atomify");
    qmlRegisterType<LAMMPSController>("Atomify", 1, 0, "LAMMPSController");
}
